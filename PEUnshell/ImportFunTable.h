#pragma once
#include <windows.h>


/*
��IMAGE_DATA_DIRECTORY�У��м�������ֶ��͵�����й�ϵ�����а�����
IMAGE_DIRECTORY_ENTRY_IMPORT��
IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT��
IMAGE_DIRECTORY_ENTRY_IAT��
IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT


IMAGE_DIRECTORY_ENTRY_IMPORT��������ͨ����֪���ĵ������PE�ļ�����ʱ������������������ݼ���������DLL����������躯���ĵ�ַ��
IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT�����󶨵�����ڵ�һ�ֵ�������ַ����������PE����ʱ��ɣ�
���һ��PE�ļ������DLL���ߺ�������ô���������ͻ����Ե���һЩ�����Գ����˰󶨵��룬�ڼ�����ǰ�������˵���������ͻ��һЩ��
IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT�����ӳٵ����һ��PE�ļ�Ҳ���ṩ�˺ܶ๦�ܣ�Ҳ�����˺ܶ�����DLL��
���ǲ���ÿ�μ��ض����õ����ṩ�����й��ܣ�Ҳ��һ�����õ�����Ҫ���������DLL������ӳٵ���ͳ����ˣ�
ֻ����һ��PE�ļ������õ���Ҫ��DLL�����DLL�Żᱻ���أ�������ֻ������ʹ��ĳ�����뺯�������������ַ�Żᱻ������
IMAGE_DIRECTORY_ENTRY_IAT�ǵ����ַ��ǰ�����������ʵ�ǵ��뺯���������������ĺ�����ַ�Ǳ�����ڵ����ַ���еġ�
*/

class ImportFunTable {
public:
	static DWORD recover(DWORD module);


};