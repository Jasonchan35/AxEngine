import AxCore_Test.PCH;
import AxCore.File;

namespace ax {

class Test_File : public UnitTestClass {
public:
	void test_case1() {
		auto srcLoc = SrcLoc::s_current();
		auto curFilename = StrView_c_str(srcLoc.file());
		auto dir = FilePath::dirname(curFilename);
		auto searchPath = Fmt("{}/Test_File_Example/**/*.*", dir);
		Array<TempString> files;
		
		File::glob(searchPath,
			[&](FileEntry& entry) {
				files.append(entry.filename);
				AX_LOG("filename = {:20}, path={}", entry.filename, entry.fullpath);
			});

		StrView expectedFiles[] = {"001.txt"_sv, "002.txt"_sv, "003.txt"_sv}; 
		AX_TEST_EQ(files, Span(expectedFiles));
	}
};

} // namespace

void Test_File() {
	using namespace ax;
	AX_TEST_RUN_CASE(Test_File::test_case1);
}
