module;

export module AxRender:ImageIO_DDS;
export import :ImageIO;

export namespace ax /*::AxRender*/ {

class ImageIO_Reader_DDS : public NonCopyable {
public:
	void load(const ImageIO::Callback& callback, ByteSpan inData);

private:
	void _readHeader	(BinIO_Reader& de, struct DDS_HEADER& hdr);
	void _readHeader10	(BinIO_Reader& de, struct DDS_HEADER_DXT10& hdr10);
};

} //namespace
