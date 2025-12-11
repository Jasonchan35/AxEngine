module;

export module AxCore.System;
export import AxCore.String;

export namespace ax {

namespace System {

Int		numberOfProcessors	();
Int		availableMemory		();

void	getMachineName		(IString_<Char> & str);

StrView	platformName();
double	uptime	();

void	setEnvVariable(StrView name, StrView value);
bool	getEnvVariable(IString& outStr, StrView	name);

Opt<TempString> envVariable(StrView name);

};

} // namespace ax 