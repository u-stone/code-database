#ifndef EXAMPLE_H_CHAP10
#define EXAMPLE_H_CHAP10

//在VC和gcc上用不到export，所以这个文件只是为了该笔记完整性而存在
/*export */template <typename T>
	T const& max__(T const& a, T const&b){
		return a < b ? b : a;//(2)
}

#endif