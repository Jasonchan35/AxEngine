module;

//#include <setjmp.h>
#include <jpeglib.h>

export module AxRender:ImageIO_JPEG;
export import :ImageIO;

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

	jmp_buf _setjmp_buffer;
	
	jpeg_decompress_struct _cinfo;
	ByteSpan _data;
	ByteSpan _remain;

	jpeg_source_mgr _srcMgr;
	jpeg_error_mgr	_errMgr;
};

} //namespace
