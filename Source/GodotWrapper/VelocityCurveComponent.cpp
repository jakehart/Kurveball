#include "VelocityCurveComponent.h"

void VelocityCurveComponent::_bind_methods()
{
	godot::ClassDB::bind_method(D_METHOD("print_type", "variant"), &VelocityCurveComponent::print_type);
}

void VelocityCurveComponent::print_type(const Variant &p_variant) const
{
	print_line(vformat("Type: %d", p_variant.get_type()));
}
