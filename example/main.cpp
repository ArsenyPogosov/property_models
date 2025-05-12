#include <iostream>

#include "property_models/model.h"

PM_PROPERTY_MODEL(TLol) {
public:
	PM_PARAMETER(int, A, 1);
	PM_PARAMETER(int, B, 2);
	PM_PARAMETER(int, C, 3);

public:
	PM_CONSTRAINT(
	    C1,
	    PM_IMPORTANCE(0),
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
	PM_CONSTRAINT(
	    C2,
	    PM_IMPORTANCE(1),
	    PM_CSM(
	        PM_IN(A, B),
	        PM_OUT(C),
	        C = A - B;
	    ),
	    PM_CSM(
	        PM_IN(A, C),
	        PM_OUT(B),
	        B = A - C;
	    ),
	    PM_CSM(
	        PM_IN(B, C),
	        PM_OUT(A),
	        A = C + B;
	    ),

	);
};

int main() {
	TLol lol;

	lol.RegisterCallback([&lol]() {
		std::cout << "Object contents:\n";
		std::cout << "  Properties:\n";
		std::cout << "    A: " << lol.A << '\n';
		std::cout << "    B: " << lol.B << '\n';
		std::cout << "    C: " << lol.C << '\n';
		std::cout << "  Constraints:\n";
		std::cout << "    - name: C1\n";
		std::cout << "      Enabled: " << std::boolalpha << lol.C1.IsEnabled() << '\n';
		std::cout << "      Fulfilled: " << std::boolalpha << lol.C1.IsFulfilled() << '\n';
		std::cout << "      Importance: " << lol.C1.GetImportance() << '\n';
		std::cout << "    - name: C2\n";
		std::cout << "      IsEnabled: " << std::boolalpha << lol.C2.IsEnabled() << '\n';
		std::cout << "      Fulfilled: " << std::boolalpha << lol.C2.IsFulfilled() << '\n';
		std::cout << "      Importance: " << lol.C2.GetImportance() << "\n\n";
	});

	{
		auto _ = lol.Freeze();
	}

	while (true) {
		std::cout << "> ";
		std::string cmd;
		if (!(std::cin >> cmd)) {
			std::cout << "\n";
			break;
		}

		if (cmd == "A" || cmd == "B" || cmd == "C") {
			int value = 0;
			std::cin >> value;
			if (cmd == "A") {
				lol.A = value;
			}
			if (cmd == "B") {
				lol.B = value;
			}
			if (cmd == "C") {
				lol.C = value;
			}
		} else if (cmd == "C1" || cmd == "C2") {
			std::string subcmd;
			std::cin >> subcmd;
			if (subcmd == "enable") {
				if (cmd == "C1") {
					lol.C1.Enable();
				} else if (cmd == "C2") {
					lol.C2.Enable();
				}
			} else if (subcmd == "disable") {
				if (cmd == "C1") {
					lol.C1.Disable();
				} else if (cmd == "C2") {
					lol.C2.Disable();
				}
			} else if (subcmd == "importance") {
				int importance = 0;
				std::cin >> importance;
				if (cmd == "C1") {
					lol.C1.SetImportance(importance);
				} else if (cmd == "C2") {
					lol.C2.SetImportance(importance);
				}
			} else {
				std::cout << "Unknown subcommand for constraint.\n";
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			}
		} else if (cmd == "exit") {
			break;
		} else {
			std::cout << "Unknown command\n";
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
	}

	return 0;
}

