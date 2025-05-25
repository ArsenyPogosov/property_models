#include <iostream>

#include "property_models/model.h"

PM_PROPERTY_MODEL(TModel) {
public:
	PM_PROPERTY(int, A, 0);
	PM_PROPERTY(int, B, 0);
	PM_PROPERTY(int, C, 0);

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
	TModel model;

	model.RegisterCallback([&model]() {
		std::cout << "Object contents:\n";
		std::cout << "  Properties:\n";
		std::cout << "    A: " << model.A << '\n';
		std::cout << "    B: " << model.B << '\n';
		std::cout << "    C: " << model.C << '\n';
		std::cout << "  Constraints:\n";
		std::cout << "    - name: C1\n";
		std::cout << "      Enabled: " << std::boolalpha << model.C1.IsEnabled() << '\n';
		std::cout << "      Fulfilled: " << std::boolalpha << model.C1.IsFulfilled() << '\n';
		std::cout << "      Importance: " << model.C1.GetImportance() << '\n';
		std::cout << "    - name: C2\n";
		std::cout << "      IsEnabled: " << std::boolalpha << model.C2.IsEnabled() << '\n';
		std::cout << "      Fulfilled: " << std::boolalpha << model.C2.IsFulfilled() << '\n';
		std::cout << "      Importance: " << model.C2.GetImportance() << "\n\n";
	});

	{
		auto _ = model.Freeze();
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
				model.A = value;
			}
			if (cmd == "B") {
				model.B = value;
			}
			if (cmd == "C") {
				model.C = value;
			}
		} else if (cmd == "C1" || cmd == "C2") {
			std::string subcmd;
			std::cin >> subcmd;
			if (subcmd == "enable") {
				if (cmd == "C1") {
					model.C1.Enable();
				} else if (cmd == "C2") {
					model.C2.Enable();
				}
			} else if (subcmd == "disable") {
				if (cmd == "C1") {
					model.C1.Disable();
				} else if (cmd == "C2") {
					model.C2.Disable();
				}
			} else if (subcmd == "importance") {
				int importance = 0;
				std::cin >> importance;
				if (cmd == "C1") {
					model.C1.SetImportance(importance);
				} else if (cmd == "C2") {
					model.C2.SetImportance(importance);
				}
			} else {
				std::cout << "Unknown subcommand for constraint.\n";
			}
		} else if (cmd == "exit") {
			break;
		} else {
			std::cout << "Unknown command\n";
		}
	}

	return 0;
}

