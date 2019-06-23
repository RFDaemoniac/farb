// based on http://donw.io/post/reflection-cpp-1/
#include "Reflection.h"


static Type* ExampleReflectableObject::type{
	{
		{
			Hash32("ExampleReflectableObject"),
			std::string("ExampleReflectableObject")
		}
	},
	sizeof(ExampleReflectableObject)
};

void SaveObject(Object* object)
{
	// Types that inherit from Object already know their type so can call
	// the overloaded SaveObject directly
	SaveObject(object, object->type);
}

void SaveObject(char* data, Type* type)
{
	// Using the description of the type, iterate over all fields in
	// the object
	for (Field* field in type->fields)
	{
		// Each field knows its offset so add that to the base address of the
		// object being saved to get at the individual field data
		char* field_data = data + field->offset;

		// If the field type is a known built-in type then we're at leaf nodes of
		// our object field hierarchies. These can be saved with explicit save
		// functions that know their type. If not, then we need to further
		// recurse until we reach a leaf node.
		Type* field_type = field->type;
		if (field_type is builtin)
			SaveBuiltin(field_data, field_type);
		else
			SaveObject(field_data, field_type);
	}
}

void SaveBuiltin(char* data, Type* type)
{
    switch (type)
    {
        case (char): WriteChar(data);
        case (short): WriteShort(data);
        case (int): WriteInt(data);
        case (float): WriteFloat(data);
        // ... etc ...
    }
}