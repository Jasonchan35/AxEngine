module;

export module AxCore.Execute;

#if 0

export namespace ax {

axStatus ax_exec		( const char* cmd, int& cmd_ret, const char*         std_in=nullptr, axIStringA*   std_out=nullptr, axIStringA*   std_err=nullptr );
axStatus ax_exec_bin	( const char* cmd, int& cmd_ret, const IByteArray* std_in=nullptr, IByteArray* std_out=nullptr, IByteArray* std_err=nullptr );

class Execute : public NonCopyable {
public:
	Execute();
	virtual ~Execute();

	virtual	bool on_stdin	( IByteArray &buf ) { return false; }
	virtual	void on_stdout	( const IByteArray &buf ) = 0;
	virtual	void on_stderr	( const IByteArray &buf ) = 0;

	axStatus	exec( const char* cmd, int& cmd_ret );

#if AX_OS_WINDOWS
	class Node : public axDListNode< Node > {
	public:
		enum {
			t_stdin,
			t_stdout,
			t_stderr,
			t_stdin_done,
			t_stdout_done,
			t_stderr_done,
		};
		int type;
		ByteArray	buf;
	};
	axAtomicQueue<Node>	q_;
#endif

};

} // namespace

#endif
