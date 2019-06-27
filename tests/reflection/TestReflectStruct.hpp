#ifndef TEST_REFLECTION_STRUCT_H
#define TEST_REFLECTION_STRUCT_H

#include <assert.h> 

#include "../RegisterTest.hpp"
#include "../../src/reflection/ReflectionDefine.hpp"
#include "TestReflectDefinitions.hpp"


namespace Farb
{

namespace Tests
{
using namespace Reflection;


class TestReflectStruct : public ITest
{
public:
	virtual bool RunTests() const override
	{
		ExampleBaseStruct e;
		ExampleEnum ee;
		char* pMembere1 = static_cast<char*>(static_cast<void*>(&(e.e1)));
		byte* pStruct = reinterpret_cast<byte*>(&e);
		assert(e.e1 == ExampleEnum::One);
		assert(e.e2 == ExampleEnum::Two);

		TypeInfo* typeInfoStruct = GetTypeInfo(e);
		TypeInfo* typeInfoMember = nullptr;
		byte* pMember = nullptr;

		auto result = typeInfoStruct->GetAtKey(pStruct, "e1");
		pMember = result.obj;
		typeInfoMember = result.typeInfo;
		farb_print(result.success, "reflect struct get member");
		assert(result.success);
		assert(pMember != nullptr);
		assert(typeInfoMember != nullptr);
		bool success = typeInfoMember->AssignString(pMember, "Zero");
		farb_print(success && e.e1 == ExampleEnum::Zero, "reflect struct assign to member enum");
		assert(success && e.e1 == ExampleEnum::Zero);
		return true;
	}
};

}

} // namespace Farb

#endif // TEST_REFLECTION_STRUCT_H
