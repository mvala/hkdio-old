#if ROOT_VERSION_CODE > ROOT_VERSION(6,0,0)
R__LOAD_LIBRARY(libHkBase.so)
void rootlogon() {
}
#else
void rootlogon() {
	gSystem->Load("libHkBase.so");
}
#endif

