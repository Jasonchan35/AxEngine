module;

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

module AxEditor;
import :AxAssimp;

namespace AxEditor {

AxAssimp::AxAssimp() {
}

void AxAssimp::openFile(StrView filename) {
	Assimp::Importer importer;
	
	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

	// Read the file. The second argument specifies post-processing flags.
	const aiScene* scene = importer.ReadFile(TempString(filename).c_str(), 
											 aiProcessPreset_TargetRealtime_MaxQuality | 
											 aiProcess_ConvertToLeftHanded); // Example flags

	// Check for errors
	if (!scene || AI_SCENE_FLAGS_INCOMPLETE == scene->mFlags || !scene->mRootNode) {
		std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	
}

} // namespace
