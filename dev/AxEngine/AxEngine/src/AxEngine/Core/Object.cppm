module;

export module AxEngine:Object;
export import :Common;

export namespace AxEngine {

using ObjectId = i64;
constexpr ObjectId ObjectId_None = 0;

class Object_CreateDesc : public NonCopyable {
public:
};

class Object : public RttiObject {
	AX_RTTI_INFO(Object, RttiObject)
public:
	using CreateDesc = Object_CreateDesc;
	
	struct Editor {
		Editor() 
		: selected(false)
		, treeNodeIsOpen(true) 
		{}
		bool selected       : 1;
		bool treeNodeIsOpen : 1;
	} editor;
	
	const String& name() const { return _name; }
	void setName(const String& name) { _name = name; }
	
protected:
//	Object(const CreateDesc& desc) {}
	ObjectId	_objectId = 0;
	String _name;
};

class ObjectManager : public NonCopyable {
public:
	static ObjectManager* s_instance(); 
	
	template<class T> void addType() { _addType(rttiOf<T>()); }
	
	struct Selection {
		void select(Object* obj);
		void deselectAll();
		
		void getSelection(IArray<SPtr<Object>> & outList);
		
	private:
		Array<WPtr<Object>> _selectedList;
	} selection;	
	
private:
	void _addType(const Rtti* rtti);
	Array<Rtti*>	_types;
	Array<Object*>	_objects;
	Array<Object*>	_rootObjects;
};

} // namespace
