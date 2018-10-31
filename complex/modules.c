#define OPS_ARE_HERE

#include <dlfcn.h>
#include <dirent.h>
#include <string.h>
#include "complex.h"
#include "modules.h"


// Глобальная переменная - массив описаний модулей
operation ops[MAX_MODULES];


// Проверка имени файла - похож ли на плагин?
int check_filename(const char* filename)
{
	return strstr(filename, ".so") != NULL;
}

// Добавление операции из модуля
int add_operation(const char* filename)
{
	char path[PATH_LENGTH] = MODULES_DIR "/";
	strcat(path, filename);

	// Используем инструментарий из dlfcn.h для загрузки внешней библитеки
	void* lib_handl = dlopen(path, RTLD_LAZY);
	if (! lib_handl) {
		return 0;
	}

	// Находим в модуле адрес структуры с информацией об этом плагине,
	// включая имя функции, выполняеющей операцию этого модуля
	mod_info_t* info = dlsym(lib_handl, INFO_VAR_STR);

	// Зная имя функции, находим её адрес
	FUNC_PROTO(func);
	func = dlsym(lib_handl, info->fname);
	if (! func) {
		return 0;
	}

	// Ищем свободный "слот" для размещения инфы о загруженном плагине
	int i = 0;
	while (ops[i].mod_hnd)
		++i;
	// Копируем полученную из библиотеки информацию
	ops[i].mod_hnd = lib_handl;
	ops[i].op_func = func;
	strcpy(ops[i].op_name, info->opname);
	ops[i].op_symbol = info->opsym;
	return 1;
}

// Просмотр заданной директории с помощью dirent.h
int load_modules()
{
	struct dirent* entry;
	int mcnt = 0;
	DIR* dir = opendir(MODULES_DIR);

	if (dir) {
		// Перебираем файлы
		while ((entry = readdir(dir)))
			// Если попадается что-то похожее на плагин, то пробуем его загрузить
			if (check_filename(entry->d_name))
				// И увеличиваем счётчик загруженных модулей в случае успеха
				mcnt += add_operation(entry->d_name);
		closedir(dir);
	}
	return mcnt;
}


void close_modules()
{
	int i = 0;
	while (ops[i].mod_hnd) {
		dlclose(ops[i].mod_hnd);
		ops[i].mod_hnd = 0;
		++i;
	}
}


