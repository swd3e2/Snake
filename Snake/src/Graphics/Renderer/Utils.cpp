#include "Utils.h"

int getInputLayoutSize(InputDataType type) {
	switch (type) {
	case Float:		return 4;
	case Float2:	return 4 * 2;
	case Float3:	return 4 * 3;
	case Float4:	return 4 * 4;
	case Int:		return 4;
	case Int2:		return 4 * 2;
	case Int3:		return 4 * 3;
	case Int4:		return 4 * 4;
	case Bool:		return 4;
	}
	return 0;
}

int getInputLayoutElementsCount(InputDataType type) {
	switch (type) {
	case Float:		return 1;
	case Float2:	return 2;
	case Float3:	return 3;
	case Float4:	return 4;
	case Int:		return 1;
	case Int2:		return 2;
	case Int3:		return 3;
	case Int4:		return 4;
	case Bool:		return 1;
	}
	return 0;
}