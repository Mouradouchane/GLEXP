
/*
// TODO : move this to system/hardware info
// TODO : implementation for LINUX
memory_info core::global_memory::get_cpu_memory_info() noexcept{

#ifdef WINDOWS
MEMORYSTATUSEX statex;
statex.dwLength = sizeof(statex); 

if (GlobalMemoryStatusEx(&statex)) {
return memory_info{
statex.ullTotalPhys,
statex.ullAvailPhys
};
}
else return memory_info();

#elif LINUX
ASSERT_EXP(0);
#else
ASSERT_EXP(0);
#endif
}
*/