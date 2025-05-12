#include <iostream>

#include "property_models/model.h"

PM_PROPERTY_MODEL(TLol) {
public:
	PM_PARAMETER(int, A, 228);
	PM_PARAMETER(int, B, 1337);

private:
	PM_PARAMETER(int, C_, 1488);

public:
	PM_CONSTRAINT(
	    ABCConstraint,
	    PM_IMPORTANCE(228),
	    PM_CSM(
	        PM_IN(A, B),
	        PM_OUT(C_),
	        C_ = A + B;
	    ),
	    PM_CSM(
	        PM_IN(A, C_),
	        PM_OUT(B),
	        B = C_ - A;
	    ),
	    PM_CSM(
	        PM_IN(B, C_),
	        PM_OUT(A),
	        A = C_ - B;
	    ),
	);
};

int main() {
	TLol lol;

	lol.RegisterCallback([&lol]() {
		static size_t i = 0;
		std::cout << "CHANGED " << i << ": " << lol.A << ' ' << lol.B << '\n';
	});

	lol.A = 3;

	std::cout << '\n';

	std::cout << lol.ABCConstraint.GetImportance() << '\n';

	return 0;
}

