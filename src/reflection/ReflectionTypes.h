// based on http://donw.io/post/reflection-cpp-1/
#ifndef FARB_REFLECTION_TYPES_H
#define FARB_REFLECTION_TYPES_H

namespace Farb
{

namespace Reflection
{

struct Name
{
	unsigned int hash;
	std::string text;
};

struct Primitive
{
	Name name;
};

struct Type : public Primitive
{
	int size;
};

struct EnumConstant : public Primitive
{
	int value;
}

struct Enum : public Type
{
	EnumConstant constants[];
};

struct Field : public Primitive
{
	Type type;
	int offset;
};

struct Function : public Primitive
{
	Field return_parameter;
	Field parameters[];
};

struct Class : public Type
{
	Field fields[];
	Function functions[];
};

struct Namespace : public Primitive
{
	Enum enums[];
	Class classes[];
	Function functions[];
};


// should this really be handled like this?
struct ReflectableObject
{
	virtual Type* GetType();
};

struct ExampleReflectableObject : public ReflectableObject
{
	static Type* type;
	virtual Type* GetType() { return type };
}


} // namespace Reflection

} // namespace Farb

#endif // FARB_REFLECTION_TYPES_H