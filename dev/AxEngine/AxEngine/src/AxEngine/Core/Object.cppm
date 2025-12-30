module;

export module AxEngine:Object;
export import :Common;

namespace AxEngine {

using ObjectId = i64;
constexpr ObjectId ObjectId_None = 0;

class Object_CreateDesc : public NonCopyable {
public:
};

class Object : public RttiObject{
	AX_RTTI_INFO(Object, RttiObject)
public:
	using CreateDesc = Object_CreateDesc;
	
protected:
//	Object(const CreateDesc& desc) {}
	ObjectId	_objectId = 0;
};

class ObjectManager : public NonCopyable {
public:
	static ObjectManager* s_instance(); 
	
private:
	Array<Object*>	_objects;
	Array<Object*>	_rootObjects;
};

} // namespace
