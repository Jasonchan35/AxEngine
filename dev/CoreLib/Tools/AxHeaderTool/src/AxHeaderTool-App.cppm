module;
#include "AxHeaderTool-pch.h"
export module AxHeaderTool.App;
export import AxHeaderTool._PCH;

export namespace ax::AxHeaderTool {

class App : public ConsoleApp {
public:
	virtual int onRun() override;
};

} //namespace