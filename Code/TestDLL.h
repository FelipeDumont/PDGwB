#pragma once
#if defined(_MSC_VER)
	#ifdef DLLPROJECT_EXPORTS
	#   define EXPORT __declspec(dllexport)
	#else
	#   define EXPORT __declspec(dllimport)
	#endif
#elif defined(__GNUC__)
	#define EXPORT __attribute__((visibility("default")))
	#define IMPORT
#endif

extern "C"{
	int EXPORT SimpleTypeReturnFun(); 			// 1.1
	int EXPORT SimpleTypeArgFun(int n); 		// 1.2
	void EXPORT ReferenceArgumentFun(int &n); 	// 1.3
	int EXPORT TestSum();

	typedef void (*SimpleCallback)();
	void EXPORT SimpleCallBackFun(SimpleCallback c);				// 2.1
	typedef int (*SimpleReturnCallback)();
	int EXPORT SimpleReturnCallbackFun(SimpleReturnCallback c); 	// 2.2
	typedef void (*SimpleArgCallback)(int n);
	void EXPORT SimpleArgCallbackFun(SimpleArgCallback c);			// 2.3

}
