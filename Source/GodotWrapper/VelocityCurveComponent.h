#pragma once

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/variant.hpp"

using namespace godot;

class VelocityCurveComponent : public Node
{
	GDCLASS(VelocityCurveComponent, Node)

protected:
	static void _bind_methods();

public:
	VelocityCurveComponent() = default;
	~VelocityCurveComponent() override = default;

	void print_type(const Variant &p_variant) const;
};
