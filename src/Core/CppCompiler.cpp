    #include "CppCompiler.h"

    #include "clang/Basic/Stack.h"
    #include "clang/Basic/TargetOptions.h"
    #include "clang/CodeGen/CodeGenAction.h"
    #include "clang/CodeGen/ObjectFilePCHContainerOperations.h"
    #include "clang/Config/config.h"
    #include "clang/Driver/DriverDiagnostic.h"
    #include "clang/Driver/Options.h"
    #include "clang/Frontend/CompilerInstance.h"
    #include "clang/Frontend/CompilerInvocation.h"
    #include "clang/Frontend/FrontendDiagnostic.h"
    #include "clang/Frontend/TextDiagnosticBuffer.h"
    #include "clang/Frontend/TextDiagnosticPrinter.h"
    #include "clang/Frontend/Utils.h"
    #include "clang/FrontendTool/Utils.h"
    #include "llvm/ADT/Statistic.h"
    #include "llvm/Config/llvm-config.h"
    #include "llvm/ExecutionEngine/ExecutionEngine.h"
    #include "llvm/InitializePasses.h"
    #include "llvm/LinkAllPasses.h"
    #include "llvm/Option/Arg.h"
    #include "llvm/Option/ArgList.h"
    #include "llvm/Option/OptTable.h"
    #include "llvm/Passes/PassBuilder.h"
    #include "llvm/Support/BuryPointer.h"
    #include "llvm/Support/Compiler.h"
    #include "llvm/Support/ErrorHandling.h"
    #include "llvm/Support/Host.h"
    #include "llvm/Support/ManagedStatic.h"
    #include "llvm/Support/Path.h"
    #include "llvm/Support/Process.h"
    #include "llvm/Support/Signals.h"
    #include "llvm/Support/TargetRegistry.h"
    #include "llvm/Support/TargetSelect.h"
    #include "llvm/Support/TimeProfiler.h"
    #include "llvm/Support/Timer.h"
    #include "llvm/Support/raw_ostream.h"
    #include "llvm/Target/TargetMachine.h"
    #include <cstdio>
    #include <sstream>

    #ifdef CLANG_HAVE_RLIMITS
    #include <sys/resource.h>
    #endif

    using namespace clang;
    using namespace llvm::opt;
    using namespace Marble::Internal;

    std::unique_ptr<llvm::orc::ThreadSafeContext> CppCompiler::context(std::make_unique<llvm::orc::ThreadSafeContext>(std::make_unique<llvm::LLVMContext>()));

    #pragma region Functions
    static void LLVMErrorHandler(void *UserData, const std::string &Message,
                                bool GenCrashDiag) {
    DiagnosticsEngine &Diags = *static_cast<DiagnosticsEngine*>(UserData);

    Diags.Report(diag::err_fe_error_backend) << Message;

    // Run the interrupt handlers to make sure any special cleanups get done, in
    // particular that we remove files registered with RemoveFileOnSignal.
    llvm::sys::RunInterruptHandlers();

    // We cannot recover from llvm errors.  When reporting a fatal error, exit
    // with status 70 to generate crash diagnostics.  For BSD systems this is
    // defined as an internal software error.  Otherwise, exit with status 1.
    llvm::sys::Process::Exit(GenCrashDiag ? 70 : 1);
    }

    #ifdef CLANG_HAVE_RLIMITS
    #if defined(__linux__) && defined(__PIE__)
    static size_t getCurrentStackAllocation() {
    // If we can't compute the current stack usage, allow for 512K of command
    // line arguments and environment.
    size_t Usage = 512 * 1024;
    if (FILE *StatFile = fopen("/proc/self/stat", "r")) {
        // We assume that the stack extends from its current address to the end of
        // the environment space. In reality, there is another string literal (the
        // program name) after the environment, but this is close enough (we only
        // need to be within 100K or so).
        unsigned long StackPtr, EnvEnd;
        // Disable silly GCC -Wformat warning that complains about length
        // modifiers on ignored format specifiers. We want to retain these
        // for documentation purposes even though they have no effect.
    #if defined(__GNUC__) && !defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wformat"
    #endif
        if (fscanf(StatFile,
                "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %*lu "
                "%*lu %*ld %*ld %*ld %*ld %*ld %*ld %*llu %*lu %*ld %*lu %*lu "
                "%*lu %*lu %lu %*lu %*lu %*lu %*lu %*lu %*llu %*lu %*lu %*d %*d "
                "%*u %*u %*llu %*lu %*ld %*lu %*lu %*lu %*lu %*lu %*lu %lu %*d",
                &StackPtr, &EnvEnd) == 2) {
    #if defined(__GNUC__) && !defined(__clang__)
    #pragma GCC diagnostic pop
    #endif
        Usage = StackPtr < EnvEnd ? EnvEnd - StackPtr : StackPtr - EnvEnd;
        }
        fclose(StatFile);
    }
    return Usage;
    }

    #include <alloca.h>

    LLVM_ATTRIBUTE_NOINLINE
    static void ensureStackAddressSpace() {
    // Linux kernels prior to 4.1 will sometimes locate the heap of a PIE binary
    // relatively close to the stack (they are only guaranteed to be 128MiB
    // apart). This results in crashes if we happen to heap-allocate more than
    // 128MiB before we reach our stack high-water mark.
    //
    // To avoid these crashes, ensure that we have sufficient virtual memory
    // pages allocated before we start running.
    size_t Curr = getCurrentStackAllocation();
    const int kTargetStack = DesiredStackSize - 256 * 1024;
    if (Curr < kTargetStack) {
        volatile char *volatile Alloc =
            static_cast<volatile char *>(alloca(kTargetStack - Curr));
        Alloc[0] = 0;
        Alloc[kTargetStack - Curr - 1] = 0;
    }
    }
    #else
    static void ensureStackAddressSpace() {}
    #endif

    /// Attempt to ensure that we have at least 8MiB of usable stack space.
    static void ensureSufficientStack() {
    struct rlimit rlim;
    if (getrlimit(RLIMIT_STACK, &rlim) != 0)
        return;

    // Increase the soft stack limit to our desired level, if necessary and
    // possible.
    if (rlim.rlim_cur != RLIM_INFINITY &&
        rlim.rlim_cur < rlim_t(DesiredStackSize)) {
        // Try to allocate sufficient stack.
        if (rlim.rlim_max == RLIM_INFINITY ||
            rlim.rlim_max >= rlim_t(DesiredStackSize))
        rlim.rlim_cur = DesiredStackSize;
        else if (rlim.rlim_cur == rlim.rlim_max)
        return;
        else
        rlim.rlim_cur = rlim.rlim_max;

        if (setrlimit(RLIMIT_STACK, &rlim) != 0 ||
            rlim.rlim_cur != DesiredStackSize)
        return;
    }

    // We should now have a stack of size at least DesiredStackSize. Ensure
    // that we can actually use that much, if necessary.
    ensureStackAddressSpace();
    }
    #else
    static void ensureSufficientStack() {}
    #endif

    /// Print supported cpus of the given target.
    static int PrintSupportedCPUs(std::string TargetStr) {
    std::string Error;
    const llvm::Target *TheTarget =
        llvm::TargetRegistry::lookupTarget(TargetStr, Error);
    if (!TheTarget) {
        llvm::errs() << Error;
        return 1;
    }

    // the target machine will handle the mcpu printing
    llvm::TargetOptions Options;
    std::unique_ptr<llvm::TargetMachine> TheTargetMachine(
        TheTarget->createTargetMachine(TargetStr, "", "+cpuhelp", Options, None));
    return 0;
    }
    #pragma endregion

    void CppCompiler::init()
    {
        auto& Registry = *llvm::PassRegistry::getPassRegistry();
    
        llvm::initializeCore(Registry);
        llvm::initializeScalarOpts(Registry);
        llvm::initializeVectorization(Registry);
        llvm::initializeIPO(Registry);
        llvm::initializeAnalysis(Registry);
        llvm::initializeTransformUtils(Registry);
        llvm::initializeInstCombine(Registry);
        llvm::initializeInstrumentation(Registry);
        llvm::initializeTarget(Registry);
    }
    void CppCompiler::deinit()
    {
    }

    std::unique_ptr<llvm::Module> CppCompiler::invoke(const std::vector<const char*>& args, const char* code, clang::Language lang)
    {
        clang::IntrusiveRefCntPtr<clang::DiagnosticIDs> diagIDs = llvm::makeIntrusiveRefCnt<clang::DiagnosticIDs>();
        clang::IntrusiveRefCntPtr<clang::DiagnosticOptions> diagnosticOptions = llvm::makeIntrusiveRefCnt<clang::DiagnosticOptions>();
        clang::TextDiagnosticPrinter textDiagnosticPrinter(llvm::outs(), diagnosticOptions.get());
    
        clang::DiagnosticsEngine diagnosticsEngine(diagIDs, diagnosticOptions, &textDiagnosticPrinter, false);
    
        clang::CompilerInstance compilerInstance;
        auto& compilerInvocation = compilerInstance.getInvocation();
    
        clang::CompilerInvocation::CreateFromArgs(compilerInvocation, llvm::ArrayRef(args.data(), args.size()), diagnosticsEngine);
    
        auto* languageOptions = compilerInvocation.getLangOpts();
        auto& preprocessorOptions = compilerInvocation.getPreprocessorOpts();
        auto& targetOptions = compilerInvocation.getTargetOpts();
        auto& frontEndOptions = compilerInvocation.getFrontendOpts();
        frontEndOptions.ShowStats = true;
        auto& headerSearchOptions = compilerInvocation.getHeaderSearchOpts();
        headerSearchOptions.Verbose = true;
        auto& codeGenOptions = compilerInvocation.getCodeGenOpts();
    
        frontEndOptions.Inputs.clear();
        frontEndOptions.Inputs.push_back(clang::FrontendInputFile(llvm::MemoryBufferRef(code, code), clang::InputKind(lang)));
    
        targetOptions.Triple = llvm::sys::getDefaultTargetTriple();
        compilerInstance.createDiagnostics(&textDiagnosticPrinter, false);
    
        clang::EmitLLVMOnlyAction action(CppCompiler::context->getContext());
    
        if (!compilerInstance.ExecuteAction(action))
        {
            __debugbreak();
        }

        std::unique_ptr<llvm::Module> module = std::move(action.takeModule());
        if (!module)
        {
            __debugbreak();
        }

        auto range = module->functions();
        for (auto it = range.begin(); it != range.end(); ++it)
            llvm::outs() << it->getName() << '\n';

        return module;
    }