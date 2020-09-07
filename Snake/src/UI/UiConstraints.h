#pragma once

enum class ValueType { Absolute, Relative };
struct Value {
	ValueType type;
	int value;

	int getNormalizedValue(size_t parent_value)
	{
		if (type == ValueType::Relative) {
			return (int)(((float)parent_value / 100.0f) * value);
		} else {
			return value;
		}
	}
};

enum class XPosition { Right, Center, Left };
class XPositionConstraint {
public:
	XPosition position = XPosition::Center;
	
	int getNormalizedValue(int parent_x_pos, int parent_width, int width)
	{
		if (position == XPosition::Left) {
			return parent_x_pos;
		} else if (position == XPosition::Center) {
			return (int)((float)parent_width / 2.0f) - ((float)width / 2.0f);
		} else if (position == XPosition::Right) {
			return parent_width - width;
		}
		assert("Unknown x pos type");
	}
};

enum class YPosition { Top, Center, Bottom };
class YPositionConstraint {
public:
	YPosition position = YPosition::Center;

	int getNormalizedValue(int parent_y_pos, int parent_height, int height, int parent_internal_height_occupied)
	{
		if (position == YPosition::Top) {
			return parent_y_pos + parent_height - height - parent_internal_height_occupied;
		} else if (position == YPosition::Center) {
			return (int)((float)parent_height / 2) - ((float)height / 2.0f);
		} else if (position == YPosition::Bottom) {
			return parent_y_pos;
		}
		assert("Unknown y pos type");
	}
};

class UiConstraints 
{
private:
	Value width;
	Value height;

	XPositionConstraint x_position;
	YPositionConstraint y_position;
public:
	void setXPos(XPositionConstraint position) { x_position = position; }
	XPositionConstraint getXPos() const { return x_position; }

	void setYPos(YPositionConstraint position) { y_position = position; }
	YPositionConstraint getYPos() const { return y_position; }

	void setWidth(Value value) { width = value; }
	Value getWidth() const { return width; }

	void setHeight(Value value) { height = value; }
	Value getHeight() const { return height; }
};