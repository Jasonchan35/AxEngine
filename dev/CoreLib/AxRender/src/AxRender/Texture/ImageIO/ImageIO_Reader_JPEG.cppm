#if 0

module;

//#include <setjmp.h>
#include <openjpeg-2.5/openjpeg.h>

export module AxRender.ImageIO_Reader_JPEG;
export import AxRender.ImageIO;

export namespace ax::AxRender {

class ImageIO_Reader_JPEG : public NonCopyable {
public:
	ImageIO_Reader_JPEG();
	~ImageIO_Reader_JPEG();

	void load(ImageIO::Callback callback, ByteSpan inData);

private:
	static void		s_init_source(j_decompress_ptr cinfo) {}
	static boolean	s_fill_input_buffer(j_decompress_ptr cinfo);
	static void		s_skip_input_data(j_decompress_ptr cinfo, long num_bytes);
	static void		s_term_source(j_decompress_ptr cinfo) {}
	static void		s_output_message(j_common_ptr cinfo);
	static void		s_error_exit(j_common_ptr cinfo);

	bool error_exit_longjmp_restore_point();

	jpeg_decompress_struct _cinfo;
	ByteSpan _data;
	ByteSpan _remain;

	jpeg_source_mgr _srcMgr;
	jpeg_error_mgr _errMgr;

#if AX_COMPILER_VC
	#pragma warning(push) 
	#pragma warning(disable: 4324) // structure was padded due to alignment specifier
#endif
	jmp_buf _setjmp_buffer;
#if AX_COMPILER_VC
	#pragma warning(pop) 
#endif
};

} //namespace

#endif