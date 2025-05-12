#include <iostream>

#include "property_models/model.h"

PM_PROPERTY_MODEL(TLol) {
public:
	PM_PARAMETER(int, A, 1);
	PM_PARAMETER(int, B, 2);
	PM_PARAMETER(int, C, 3);

public:
	PM_CONSTRAINT(
	    ABCConstraint,
	    PM_IMPORTANCE(228),
	    PM_CSM(
	        PM_IN(A, B),
	        PM_OUT(C),
	        C = A + B;
	    ),
	    PM_CSM(
	        PM_IN(A, C),
	        PM_OUT(B),
	        B = C - A;
	    ),
	    PM_CSM(
	        PM_IN(B, C),
	        PM_OUT(A),
	        A = C - B;
	    ),
	);
};

int main() {
	TLol lol;

	lol.RegisterCallback([&lol]() {
		static size_t i = 0;
		std::cout << "CHANGED " << i++ << ": " << lol.A << ' ' << lol.B << ' ' << lol.C << '\n';
	});

	char h = '0';
	int x = 0;
	while (std::cin >> h >> x) {
		if (h == 'A') {
			lol.A = x;
		}
		if (h == 'B') {
			lol.B = x;
		}
		if (h == 'C') {
			lol.C = x;
		}
	}

	std::cout << lol.ABCConstraint.GetImportance() << '\n';

	return 0;
}

