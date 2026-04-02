#pragma once
#include <vector>
#include <array>

// 配列補助関数群ヘッダ

// 指定した要素が含まれるかチェックする関数
template<typename Container, typename T>
bool Contain(const Container& array, const T& value)
{
	return (std::find(array.begin(), array.end(), value) != array.end());
}

// 指定した要素のインデックスを返す関数
template<typename Container, typename T>
int GetArrayIndex(const Container& array, const T& value)
{
	auto iterator = std::find(array.begin(), array.end(), value);

	if (iterator == array.end()) return -1;

	return static_cast<int>(std::distance(array.begin(), iterator));
}
