// RUN: heir-opt --verify-diagnostics %s

#ideal = #polynomial.polynomial<1 + x**1024>
#ideal_2 = #polynomial.polynomial<1 + x**2048>

// Below we use random 32-bit primes
#ring_1 = #polynomial.ring<cmod=3721063133, ideal=#ideal>
#ring_2 = #polynomial.ring<cmod=2737228591, ideal=#ideal>
#ring_3 = #polynomial.ring<cmod=3180146689, ideal=#ideal>
#ring_bad = #polynomial.ring<cmod=3180146689, ideal=#ideal_2>

!poly_ty_1 = !polynomial.polynomial<#ring_1>
!poly_ty_2 = !polynomial.polynomial<#ring_2>
!poly_ty_3 = !polynomial.polynomial<#ring_3>

!ty = !rns.rns<!poly_ty_1, !poly_ty_2, !poly_ty_3>

func.func @test_syntax(%arg0: !ty) -> !ty {
  return %arg0 : !ty
}

!poly_ty_bad = !polynomial.polynomial<#ring_bad>
// expected-error@+1 {{RNS type has incompatible basis types}}
!ty_bad = !rns.rns<!poly_ty_1, !poly_ty_2, !poly_ty_bad>
