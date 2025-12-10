module;

export module AxCore.OS;
export import AxCore.String;

export namespace ax {

class OS {
public:
	static	Int		numberOfProcessors	();
	static	Int		availableMemory		();

			void	getMachineName		(IString_<Char> & str);

	static	StrView	platformName();
	static	double	uptime	();

	static	String	getenv(StrView	name);
	static	void	setenv(StrView name, StrView value);
};

} // namespace ax 