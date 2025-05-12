# Property Models

# Прототип интерфейса

```c++
struct Squere: private PropertyModel {
public:
	PROPERTY(int, x1);
	PROPERTY(int, y1);
	PROPERTY(int, x2);
	PROPERTY(int, y2);
	PROPERTY(int, length);
	PROPERTY(double, diagonal);
	PROPERTY(int, area);

private:
	PROPERTY(int, width);
	PROPERTY(int, height);

private:
	// manual constraint
	CONSTRAINT(
		rightWidth1,  // name
		0,            // importance
		{
			CSM(
				IN(x1, y1),
				OUT(width),
				width = y1 - x1;
				// y1 = x1 + width: compilation error
				// width = height: compilation error
			),
			CSM(
				IN(x1, width),
				OUT(y1),
				y1 = x1 + width;
			),
			CSM(
				IN(y1, width),
				OUT(x1),
				x1 = y1 - width;
			),
		}
	);

	// premade constraint
	CONSTRAINT(
		rightWidth2,
		0,
		IS_SUM(y1, x1, width)
	);

	CONSTRAINT(
		isSquare,
		0,
		ARE_EQUAL(width, height, length)
	);

	...

public:
	CONSTRAINT(
		aboveZero,
		228,
		IS_GREATER(y1, 0)
	);
}

int main() {
	Square square;
	square.length = 15;
	cout << square.x1 << '\n';

	square.y1.OnSet([](const Squere &squere) -> void {
		cout << squere.y1 << '\n';
	});

	cout << squere.aboveZero.IsFulfilled() << '\n';
	squere.aboveZero.SetImportance(1337);

	{
		auto _ = squere.Freeze();
		squere.x1 = 1;
		squere.y1 = 2;
		squere.x2 = 3;
		squere.y2 = 4;
	}
	cout << squere.x1 << '\n';
}
```

