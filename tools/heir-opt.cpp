#include <cstdlib>
#include <string>

#include "include/Conversion/BGVToOpenfhe/BGVToOpenfhe.h"
#include "include/Conversion/BGVToPolynomial/BGVToPolynomial.h"
#include "include/Conversion/CGGIToTfheRust/CGGIToTfheRust.h"
#include "include/Conversion/CGGIToTfheRustBool/CGGIToTfheRustBool.h"
#include "include/Conversion/CombToCGGI/CombToCGGI.h"
#include "include/Conversion/MemrefToArith/MemrefToArith.h"
#include "include/Conversion/PolynomialToStandard/PolynomialToStandard.h"
#include "include/Conversion/SecretToBGV/SecretToBGV.h"
#include "include/Dialect/BGV/IR/BGVDialect.h"
#include "include/Dialect/CGGI/IR/CGGIDialect.h"
#include "include/Dialect/CGGI/Transforms/Passes.h"
#include "include/Dialect/Comb/IR/CombDialect.h"
#include "include/Dialect/LWE/IR/LWEDialect.h"
#include "include/Dialect/LWE/Transforms/Passes.h"
#include "include/Dialect/Openfhe/IR/OpenfheDialect.h"
#include "include/Dialect/PolyExt/IR/PolyExtDialect.h"
#include "include/Dialect/Polynomial/IR/PolynomialDialect.h"
#include "include/Dialect/RNS/IR/RNSDialect.h"
#include "include/Dialect/Secret/IR/SecretDialect.h"
#include "include/Dialect/Secret/Transforms/DistributeGeneric.h"
#include "include/Dialect/Secret/Transforms/Passes.h"
#include "include/Dialect/TensorExt/IR/TensorExtDialect.h"
#include "include/Dialect/TensorExt/Transforms/CollapseInsertionChains.h"
#include "include/Dialect/TensorExt/Transforms/InsertRotate.h"
#include "include/Dialect/TensorExt/Transforms/Passes.h"
#include "include/Dialect/TensorExt/Transforms/RotateAndReduce.h"
#include "include/Dialect/TfheRust/IR/TfheRustDialect.h"
#include "include/Dialect/TfheRustBool/IR/TfheRustBoolDialect.h"
#include "include/Transforms/ElementwiseToAffine/ElementwiseToAffine.h"
#include "include/Transforms/ForwardStoreToLoad/ForwardStoreToLoad.h"
#include "include/Transforms/FullLoopUnroll/FullLoopUnroll.h"
#include "include/Transforms/Secretize/Passes.h"
#include "include/Transforms/StraightLineVectorizer/StraightLineVectorizer.h"
#include "include/Transforms/UnusedMemRef/UnusedMemRef.h"
#include "llvm/include/llvm/Support/CommandLine.h"  // from @llvm-project
#include "llvm/include/llvm/Support/raw_ostream.h"  // from @llvm-project
#include "mlir/include/mlir/Conversion/AffineToStandard/AffineToStandard.h"  // from @llvm-project
#include "mlir/include/mlir/Conversion/ArithToLLVM/ArithToLLVM.h"  // from @llvm-project
#include "mlir/include/mlir/Conversion/BufferizationToMemRef/BufferizationToMemRef.h"  // from @llvm-project
#include "mlir/include/mlir/Conversion/ConvertToLLVM/ToLLVMPass.h"  // from @llvm-project
#include "mlir/include/mlir/Conversion/IndexToLLVM/IndexToLLVM.h"  // from @llvm-project
#include "mlir/include/mlir/Conversion/MemRefToLLVM/MemRefToLLVM.h"  // from @llvm-project
#include "mlir/include/mlir/Conversion/TensorToLinalg/TensorToLinalgPass.h"  // from @llvm-project
#include "mlir/include/mlir/Conversion/TosaToArith/TosaToArith.h"  // from @llvm-project
#include "mlir/include/mlir/Conversion/TosaToLinalg/TosaToLinalg.h"  // from @llvm-project
#include "mlir/include/mlir/Conversion/TosaToTensor/TosaToTensor.h"  // from @llvm-project
#include "mlir/include/mlir/Dialect/Affine/IR/AffineOps.h"  // from @llvm-project
#include "mlir/include/mlir/Dialect/Affine/Passes.h"   // from @llvm-project
#include "mlir/include/mlir/Dialect/Arith/IR/Arith.h"  // from @llvm-project
#include "mlir/include/mlir/Dialect/Arith/Transforms/Passes.h"  // from @llvm-project
#include "mlir/include/mlir/Dialect/Bufferization/Transforms/Passes.h"  // from @llvm-project
#include "mlir/include/mlir/Dialect/Func/IR/FuncOps.h"  // from @llvm-project
#include "mlir/include/mlir/Dialect/Func/Transforms/Passes.h"  // from @llvm-project
#include "mlir/include/mlir/Dialect/LLVMIR/LLVMDialect.h"  // from @llvm-project
#include "mlir/include/mlir/Dialect/Linalg/Passes.h"       // from @llvm-project
#include "mlir/include/mlir/Dialect/MemRef/IR/MemRef.h"    // from @llvm-project
#include "mlir/include/mlir/Dialect/MemRef/Transforms/Passes.h"  // from @llvm-project
#include "mlir/include/mlir/Dialect/SCF/IR/SCF.h"  // from @llvm-project
#include "mlir/include/mlir/Dialect/Tensor/Transforms/Passes.h"  // from @llvm-project
#include "mlir/include/mlir/Dialect/Tosa/IR/TosaOps.h"     // from @llvm-project
#include "mlir/include/mlir/InitAllDialects.h"             // from @llvm-project
#include "mlir/include/mlir/InitAllExtensions.h"           // from @llvm-project
#include "mlir/include/mlir/InitAllPasses.h"               // from @llvm-project
#include "mlir/include/mlir/Pass/PassManager.h"            // from @llvm-project
#include "mlir/include/mlir/Pass/PassOptions.h"            // from @llvm-project
#include "mlir/include/mlir/Pass/PassRegistry.h"           // from @llvm-project
#include "mlir/include/mlir/Tools/mlir-opt/MlirOptMain.h"  // from @llvm-project
#include "mlir/include/mlir/Transforms/Passes.h"           // from @llvm-project

#ifndef HEIR_NO_YOSYS
#include "include/Transforms/YosysOptimizer/YosysOptimizer.h"
#endif

using namespace mlir;
using namespace tosa;
using namespace heir;
using mlir::func::FuncOp;

void tosaToLinalg(OpPassManager &manager) {
  manager.addNestedPass<FuncOp>(createTosaToLinalgNamed());
  manager.addNestedPass<FuncOp>(createTosaToLinalg());
  manager.addNestedPass<FuncOp>(createTosaToArith(true, false));
  manager.addNestedPass<FuncOp>(createTosaToTensor());
  manager.addPass(bufferization::createEmptyTensorToAllocTensorPass());
  manager.addNestedPass<FuncOp>(createLinalgDetensorizePass());
  manager.addPass(createConvertTensorToLinalgPass());
  manager.addPass(bufferization::createEmptyTensorToAllocTensorPass());
}

void oneShotBufferize(OpPassManager &manager) {
  // One-shot bufferize, from
  // https://mlir.llvm.org/docs/Bufferization/#ownership-based-buffer-deallocation
  bufferization::OneShotBufferizationOptions bufferizationOptions;
  bufferizationOptions.bufferizeFunctionBoundaries = true;
  manager.addPass(
      bufferization::createOneShotBufferizePass(bufferizationOptions));
  manager.addPass(memref::createExpandReallocPass());
  manager.addPass(bufferization::createOwnershipBasedBufferDeallocationPass());
  manager.addPass(createCanonicalizerPass());
  manager.addPass(bufferization::createBufferDeallocationSimplificationPass());
  manager.addPass(bufferization::createLowerDeallocationsPass());
  manager.addPass(createCSEPass());
  manager.addPass(createCanonicalizerPass());
}

void tosaPipelineBuilder(OpPassManager &manager) {
  // TOSA to linalg
  tosaToLinalg(manager);
  // Bufferize
  manager.addNestedPass<FuncOp>(createLinalgBufferizePass());
  manager.addNestedPass<FuncOp>(tensor::createTensorBufferizePass());
  manager.addPass(arith::createArithBufferizePass());
  manager.addPass(func::createFuncBufferizePass());
  manager.addNestedPass<FuncOp>(bufferization::createFinalizingBufferizePass());
  // Affine
  manager.addNestedPass<FuncOp>(createConvertLinalgToAffineLoopsPass());
  manager.addNestedPass<FuncOp>(memref::createExpandStridedMetadataPass());
  manager.addNestedPass<FuncOp>(affine::createAffineExpandIndexOpsPass());
  manager.addNestedPass<FuncOp>(memref::createExpandOpsPass());
  manager.addNestedPass<FuncOp>(affine::createSimplifyAffineStructuresPass());
  manager.addPass(memref::createFoldMemRefAliasOpsPass());
  manager.addPass(createExpandCopyPass());
  manager.addPass(createExtractLoopBodyPass());
  manager.addPass(createUnrollAndForwardPass());
  // Cleanup
  manager.addPass(createMemrefGlobalReplacePass());
  arith::ArithIntNarrowingOptions options;
  options.bitwidthsSupported = {4, 8, 16};
  manager.addPass(arith::createArithIntNarrowing(options));
  manager.addPass(createCanonicalizerPass());
  manager.addPass(createSCCPPass());
  manager.addPass(createCSEPass());
  manager.addPass(createSymbolDCEPass());
}

void polynomialToLLVMPipelineBuilder(OpPassManager &manager) {
  // Poly
  manager.addPass(createElementwiseToAffine());
  manager.addPass(polynomial::createPolynomialToStandard());
  manager.addPass(createCanonicalizerPass());

  // Linalg
  manager.addNestedPass<FuncOp>(createConvertElementwiseToLinalgPass());
  // Needed to lower affine.map and affine.apply
  manager.addNestedPass<FuncOp>(affine::createAffineExpandIndexOpsPass());
  manager.addNestedPass<FuncOp>(affine::createSimplifyAffineStructuresPass());
  manager.addPass(createLowerAffinePass());
  manager.addNestedPass<FuncOp>(memref::createExpandOpsPass());
  manager.addNestedPass<FuncOp>(memref::createExpandStridedMetadataPass());

  // Bufferize
  oneShotBufferize(manager);

  // Linalg must be bufferized before it can be lowered
  // But lowering to loops also re-introduces affine.apply, so re-lower that
  manager.addNestedPass<FuncOp>(createConvertLinalgToLoopsPass());
  manager.addPass(createLowerAffinePass());
  manager.addPass(createBufferizationToMemRefPass());

  // Cleanup
  manager.addPass(createCanonicalizerPass());
  manager.addPass(createSCCPPass());
  manager.addPass(createCSEPass());
  manager.addPass(createSymbolDCEPass());

  // ToLLVM
  manager.addPass(arith::createArithExpandOpsPass());
  manager.addPass(createConvertSCFToCFPass());
  manager.addNestedPass<FuncOp>(memref::createExpandStridedMetadataPass());
  manager.addPass(createConvertToLLVMPass());

  // Cleanup
  manager.addPass(createCanonicalizerPass());
  manager.addPass(createSCCPPass());
  manager.addPass(createCSEPass());
  manager.addPass(createSymbolDCEPass());
}

void heirSIMDVectorizerPipelineBuilder(OpPassManager &manager) {
  // For now we unroll loops to enable insert-rotate, but we would like to be
  // smarter about this and do an affine loop analysis.
  // TODO(#589): avoid unrolling loops
  manager.addPass(createFullLoopUnroll());

  // This canonicalize is required in this position for a relatively nuanced
  // reason. insert-rotate doesn't have general match support. In particular,
  // if a tensor extract from a secret is combined with a tensor extract from
  // a constant 2D tensor (e.g., the weight matrix of a convolution), then
  // insert-rotate won't be able to tell the difference and understand that
  // the extracted value from the 2D tensor should be splatted. This
  // canonicalize pass converts a constant weight matrix into the underlying
  // arith.constant values, which are supported as a splattable non-tensor
  // input in insert-rotate.
  // TODO(#586): find a more robust solution
  manager.addPass(createCanonicalizerPass());

  // Insert rotations aligned to slot targets. Future work should provide
  // alternative methods to optimally align rotations, and allow the user to
  // configure this via pipeline options.
  manager.addPass(tensor_ext::createInsertRotate());
  manager.addPass(createCSEPass());
  manager.addPass(createCanonicalizerPass());

  manager.addPass(tensor_ext::createCollapseInsertionChains());
  manager.addPass(createCSEPass());
  manager.addPass(createSCCPPass());
  manager.addPass(createCanonicalizerPass());

  manager.addPass(tensor_ext::createRotateAndReduce());
  manager.addPass(createCSEPass());
  manager.addPass(createSCCPPass());
  manager.addPass(createCanonicalizerPass());
}

#ifndef HEIR_NO_YOSYS
struct TosaToBooleanTfheOptions
    : public PassPipelineOptions<TosaToBooleanTfheOptions> {
  PassOptions::Option<bool> abcFast{*this, "abc-fast",
                                    llvm::cl::desc("Run abc in fast mode."),
                                    llvm::cl::init(false)};

  PassOptions::Option<int> unrollFactor{
      *this, "unroll-factor",
      llvm::cl::desc("Unroll loops by a given factor before optimizing. A "
                     "value of zero (default) prevents unrolling."),
      llvm::cl::init(0)};

  PassOptions::Option<std::string> entryFunction{
      *this, "entry-function", llvm::cl::desc("Entry function to secretize"),
      llvm::cl::init("main")};
};

void tosaToBooleanTfhePipeline(const std::string &yosysFilesPath,
                               const std::string &abcPath) {
  PassPipelineRegistration<TosaToBooleanTfheOptions>(
      "tosa-to-boolean-tfhe", "Arithmetic modules to boolean tfhe-rs pipeline.",
      [yosysFilesPath, abcPath](OpPassManager &pm,
                                const TosaToBooleanTfheOptions &options) {
        // Secretize inputs
        pm.addPass(createSecretize(SecretizeOptions{options.entryFunction}));

        // TOSA to linalg
        tosaToLinalg(pm);

        // Bufferize
        oneShotBufferize(pm);

        // Affine
        pm.addNestedPass<FuncOp>(createConvertLinalgToAffineLoopsPass());
        pm.addNestedPass<FuncOp>(memref::createExpandStridedMetadataPass());
        pm.addNestedPass<FuncOp>(affine::createAffineExpandIndexOpsPass());
        pm.addNestedPass<FuncOp>(memref::createExpandOpsPass());
        pm.addNestedPass<FuncOp>(affine::createSimplifyAffineStructuresPass());
        pm.addPass(memref::createFoldMemRefAliasOpsPass());
        pm.addPass(createExpandCopyPass());

        // Cleanup
        pm.addPass(createMemrefGlobalReplacePass());
        arith::ArithIntNarrowingOptions arithOps;
        arithOps.bitwidthsSupported = {4, 8, 16};
        pm.addPass(arith::createArithIntNarrowing(arithOps));
        pm.addPass(createCanonicalizerPass());
        pm.addPass(createSCCPPass());
        pm.addPass(createCSEPass());
        pm.addPass(createSymbolDCEPass());

        // Wrap with secret.generic and then distribute-generic.
        pm.addPass(createWrapGeneric());
        auto distributeOpts = secret::SecretDistributeGenericOptions{};
        distributeOpts.opsToDistribute = {"affine.for", "affine.load",
                                          "affine.store", "memref.get_global"};
        pm.addPass(secret::createSecretDistributeGeneric(distributeOpts));
        pm.addPass(createCanonicalizerPass());

        // Booleanize and Yosys Optimize
        pm.addPass(createYosysOptimizer(yosysFilesPath, abcPath,
                                        options.abcFast, options.unrollFactor));

        // Lower combinational circuit to CGGI
        pm.addPass(mlir::createCSEPass());
        pm.addPass(secret::createSecretDistributeGeneric());
        pm.addPass(comb::createCombToCGGI());

        // CGGI to Tfhe-Rust exit dialect
        pm.addPass(createCGGIToTfheRust());
        pm.addPass(createCanonicalizerPass());
        pm.addPass(createCSEPass());

        // Cleanup loads and stores
        pm.addPass(createExpandCopyPass(
            ExpandCopyPassOptions{.disableAffineLoop = true}));
        pm.addPass(memref::createFoldMemRefAliasOpsPass());
        pm.addPass(createForwardStoreToLoad());
        pm.addPass(createCanonicalizerPass());
        pm.addPass(createCSEPass());
        pm.addPass(createSCCPPass());
      });
}

struct TosaToBooleanFpgaTfheOptions
    : public PassPipelineOptions<TosaToBooleanFpgaTfheOptions> {
  PassOptions::Option<bool> abcFast{*this, "abc-fast",
                                    llvm::cl::desc("Run abc in fast mode."),
                                    llvm::cl::init(false)};

  PassOptions::Option<int> unrollFactor{
      *this, "unroll-factor",
      llvm::cl::desc("Unroll loops by a given factor before optimizing. A "
                     "value of zero (default) prevents unrolling."),
      llvm::cl::init(0)};

  PassOptions::Option<std::string> entryFunction{
      *this, "entry-function", llvm::cl::desc("Entry function to secretize"),
      llvm::cl::init("main")};
};

void tosaToBooleanFpgaTfhePipeline(const std::string &yosysFilesPath,
                                   const std::string &abcPath) {
  PassPipelineRegistration<TosaToBooleanFpgaTfheOptions>(
      "tosa-to-boolean-fpga-tfhe",
      "Arithmetic modules to boolean tfhe-rs for FPGA backend pipeline.",
      [yosysFilesPath, abcPath](OpPassManager &pm,
                                const TosaToBooleanFpgaTfheOptions &options) {
        // Secretize inputs
        pm.addPass(createSecretize(SecretizeOptions{options.entryFunction}));

        // TOSA to linalg
        tosaToLinalg(pm);

        // Bufferize
        oneShotBufferize(pm);

        // Affine
        pm.addNestedPass<FuncOp>(createConvertLinalgToAffineLoopsPass());
        pm.addNestedPass<FuncOp>(memref::createExpandStridedMetadataPass());
        pm.addNestedPass<FuncOp>(affine::createAffineExpandIndexOpsPass());
        pm.addNestedPass<FuncOp>(memref::createExpandOpsPass());
        pm.addNestedPass<FuncOp>(affine::createSimplifyAffineStructuresPass());
        pm.addPass(memref::createFoldMemRefAliasOpsPass());
        pm.addPass(createExpandCopyPass());
        pm.addNestedPass<FuncOp>(affine::createAffineLoopNormalizePass(true));
        pm.addNestedPass<FuncOp>(affine::createLoopFusionPass(
            0, 0, true, affine::FusionMode::Greedy));
        pm.addPass(affine::createAffineScalarReplacementPass());
        pm.addPass(createForwardStoreToLoad());

        // Cleanup
        pm.addPass(createMemrefGlobalReplacePass());
        arith::ArithIntNarrowingOptions arithOps;
        arithOps.bitwidthsSupported = {4, 8, 16};
        pm.addPass(arith::createArithIntNarrowing(arithOps));
        pm.addPass(createCanonicalizerPass());
        pm.addPass(createSCCPPass());
        pm.addPass(createCSEPass());
        pm.addPass(createSymbolDCEPass());

        pm.addPass(createWrapGeneric());
        auto distributeOpts = secret::SecretDistributeGenericOptions{};
        distributeOpts.opsToDistribute = {"affine.for",     "affine.load",
                                          "memref.load",    "memref.store",
                                          "affine.store",   "memref.get_global",
                                          "memref.dealloc", "memref.alloc"};
        pm.addPass(secret::createSecretDistributeGeneric(distributeOpts));
        pm.addPass(createCanonicalizerPass());

        // Booleanize and Yosys Optimize
        pm.addPass(createYosysOptimizer(yosysFilesPath, abcPath,
                                        options.abcFast, options.unrollFactor,
                                        Mode::Boolean));

        // Lower combinational circuit to CGGI
        pm.addPass(createForwardStoreToLoad());
        pm.addPass(mlir::createCSEPass());
        pm.addPass(secret::createSecretDistributeGeneric());
        pm.addPass(comb::createCombToCGGI());
        // Cleanup CombToCGGI
        pm.addPass(createExpandCopyPass(
            ExpandCopyPassOptions{.disableAffineLoop = true}));
        pm.addPass(memref::createFoldMemRefAliasOpsPass());
        pm.addPass(createForwardStoreToLoad());
        pm.addPass(createRemoveUnusedMemRef());

        pm.addPass(createStraightLineVectorizer(
            StraightLineVectorizerOptions{.dialect = "cggi"}));
        pm.addPass(createCanonicalizerPass());
        pm.addPass(createCSEPass());
        pm.addPass(createSCCPPass());

        // CGGI to Tfhe-Rust exit dialect
        pm.addPass(createCGGIToTfheRustBool());
        pm.addPass(createCanonicalizerPass());
        pm.addPass(createCSEPass());
        pm.addPass(createSCCPPass());
      });
}
#endif

struct MlirToOpenFheBgvPipelineOptions
    : public PassPipelineOptions<MlirToOpenFheBgvPipelineOptions> {
  PassOptions::Option<std::string> entryFunction{
      *this, "entry-function", llvm::cl::desc("Entry function to secretize"),
      llvm::cl::init("main")};
  PassOptions::Option<int> ciphertextDegree{
      *this, "ciphertext-degree",
      llvm::cl::desc("The degree of the polynomials to use for ciphertexts; "
                     "equivalently, the number of messages that can be packed "
                     "into a single ciphertext."),
      llvm::cl::init(1024)};
};

void mlirToOpenFheBgvPipelineBuilder(
    OpPassManager &pm, const MlirToOpenFheBgvPipelineOptions &options) {
  // Secretize inputs
  pm.addPass(createSecretize(SecretizeOptions{options.entryFunction}));
  pm.addPass(createWrapGeneric());
  pm.addPass(createCanonicalizerPass());
  pm.addPass(createCSEPass());

  // Vectorize and optimize rotations
  heirSIMDVectorizerPipelineBuilder(pm);

  // Prepare to lower to BGV
  pm.addPass(secret::createSecretDistributeGeneric());
  pm.addPass(createCanonicalizerPass());

  // Lower to BGV
  auto secretToBgvOpts = SecretToBGVOptions{};
  secretToBgvOpts.polyModDegree = options.ciphertextDegree;
  pm.addPass(createSecretToBGV(secretToBgvOpts));

  // Lower to openfhe
  pm.addPass(bgv::createBGVToOpenfhe());
}

int main(int argc, char **argv) {
  mlir::DialectRegistry registry;

  registry.insert<bgv::BGVDialect>();
  registry.insert<cggi::CGGIDialect>();
  registry.insert<comb::CombDialect>();
  registry.insert<lwe::LWEDialect>();
  registry.insert<openfhe::OpenfheDialect>();
  registry.insert<poly_ext::PolyExtDialect>();
  registry.insert<polynomial::PolynomialDialect>();
  registry.insert<rns::RNSDialect>();
  registry.insert<secret::SecretDialect>();
  registry.insert<tensor_ext::TensorExtDialect>();
  registry.insert<tfhe_rust::TfheRustDialect>();
  registry.insert<tfhe_rust_bool::TfheRustBoolDialect>();

  // Add expected MLIR dialects to the registry.
  registry.insert<affine::AffineDialect>();
  registry.insert<arith::ArithDialect>();
  registry.insert<func::FuncDialect>();
  registry.insert<memref::MemRefDialect>();
  registry.insert<scf::SCFDialect>();
  registry.insert<tensor::TensorDialect>();
  registry.insert<TosaDialect>();
  registry.insert<LLVM::LLVMDialect>();
  registerAllDialects(registry);
  registerAllExtensions(registry);

  // Register MLIR core passes to build pipeline.
  registerAllPasses();

  // Custom passes in HEIR
  cggi::registerCGGIPasses();
  lwe::registerLWEPasses();
  secret::registerSecretPasses();
  tensor_ext::registerTensorExtPasses();
  registerElementwiseToAffinePasses();
  registerSecretizePasses();
  registerFullLoopUnrollPasses();
  registerForwardStoreToLoadPasses();
  registerStraightLineVectorizerPasses();
  registerUnusedMemRefPasses();
  // Register yosys optimizer pipeline if configured.
#ifndef HEIR_NO_YOSYS
  const char *abcEnvPath = std::getenv("HEIR_ABC_BINARY");
  const char *yosysRunfilesEnvPath = std::getenv("HEIR_YOSYS_SCRIPTS_DIR");
  if (abcEnvPath == nullptr || yosysRunfilesEnvPath == nullptr) {
    llvm::errs() << "yosys optimizer deps not found, please set "
                    "HEIR_ABC_PATH and HEIR_YOSYS_LIBS; otherwise, set "
                    "HEIR_NO_YOSYS=1\n";
    return EXIT_FAILURE;
  }
  mlir::heir::registerYosysOptimizerPipeline(yosysRunfilesEnvPath, abcEnvPath);
  tosaToBooleanTfhePipeline(yosysRunfilesEnvPath, abcEnvPath);
  tosaToBooleanFpgaTfhePipeline(yosysRunfilesEnvPath, abcEnvPath);
#endif

  // Dialect conversion passes in HEIR
  bgv::registerBGVToPolynomialPasses();
  bgv::registerBGVToOpenfhePasses();
  comb::registerCombToCGGIPasses();
  polynomial::registerPolynomialToStandardPasses();
  registerCGGIToTfheRustPasses();
  registerCGGIToTfheRustBoolPasses();
  registerSecretToBGVPasses();

  PassPipelineRegistration<>(
      "heir-tosa-to-arith",
      "Run passes to lower TOSA models with stripped quant types to arithmetic",
      tosaPipelineBuilder);

  PassPipelineRegistration<>(
      "heir-polynomial-to-llvm",
      "Run passes to lower the polynomial dialect to LLVM",
      polynomialToLLVMPipelineBuilder);

  PassPipelineRegistration<>(
      "heir-simd-vectorizer",
      "Run scheme-agnostic passes to convert FHE programs that operate on "
      "scalar types to equivalent programs that operate on vectors and use "
      "tensor_ext.rotate",
      heirSIMDVectorizerPipelineBuilder);

  PassPipelineRegistration<MlirToOpenFheBgvPipelineOptions>(
      "mlir-to-openfhe-bgv-pipeline",
      "Convert a func using standard MLIR dialects to FHE using BGV and export "
      "to OpenFHE C++ code.",
      mlirToOpenFheBgvPipelineBuilder);

  return asMainReturnCode(
      MlirOptMain(argc, argv, "HEIR Pass Driver", registry));
}
