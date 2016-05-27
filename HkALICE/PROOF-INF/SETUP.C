R__LOAD_LIBRARY(libHkALICE.so)
Int_t SETUP() {
	if (!TClass::GetClass("HkAliESDFilter")) {
		return 1;
	}
	return 0;
}
