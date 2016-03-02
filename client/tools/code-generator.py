#!/usr/bin/python
import glob, os

TAB_SPACE = "  "

def add_warning_info():
	str = "//this code is generated by python script, you should not edit it.\n"
	str += "//author: fengxp\n"
	str += "//email: xiaopeifenng AT hotmail.com\n"
	str += "//copyright: happynetwork\n\n\n"
	return str

def find_sqlconf_file(directory='.'):
	os.chdir("/home/fengxp/dbproxy/client/test")
	files = []
	for file in glob.glob("*.dbsql"):
		files.append(file) 
	return files

def deal_items_header(namespace, item):
	if type(item) != dict:
		print "error"
	name = item["name"]	
	database = item["db"]
	operatetype = item["type"]
	sql = item["sql"]
	args = item["arguments"]
	
	select_fields = item.get("fields")
	filestring = ""
	if operatetype == "select":
		filestring += "typedef struct " + namespace + "_" + name + "_field{\n"
		for a in select_fields:
			filestring += TAB_SPACE
			if a[1] == "string":
				filestring += "char " + a[0] + "[1024];\n"
			else:
				filestring += a[1] + " "
				filestring += a[0] + ";\n"
		filestring += "}" + namespace + "_" + name + "_field_t;\n\n"
	
	filestring += "typedef struct " + namespace + "_" + name + "_res{\n"
	filestring += TAB_SPACE; filestring += "int code;\n"
	filestring += TAB_SPACE; filestring += "char desc[1024];\n"
	if operatetype == "select":
		filestring += TAB_SPACE
		filestring += "int rows_count;\n"	
		filestring += TAB_SPACE
		filestring += namespace + "_" + name + "_field_t* rows;\n"
	filestring += "}" + namespace + "_" + name + "_res_t;\n\n"
#add sync function
	function_name = namespace + "_" + name
	return_name = namespace + "_" + name + "_res_t"
	filestring += return_name + " " + function_name
	arg_list = "(DBProxyClient* client,"
	for a in args:
		x = ""
		if a[1] == "string":
			x = "const char*"
		else:
			x = a[1]
		arg_list += " "	+ x + " " + a[0] + ","

	filestring += arg_list
	filestring = filestring.rstrip(',')
	filestring += ");\n"

#add async function
	filestring += "void " + function_name + "_async"
	filestring += arg_list
	filestring += " query_call_cb_t func, void* arg);\n\n"

	return filestring

def deal_item_source(namespace, item):
	if type(item) != dict:
		print "error"		
	name = item["name"]
	database = item["db"]
	operatetype = item["type"]
	args = item["arguments"]
	sql = item["sql"]
	select_fields = item.get("fields")
	function_name = namespace + "_" + name + "_res_t "
	function_name += namespace + "_" + name
	function_name += "(DBProxyClient* client,"
	for a in args:
		if a[1] == "string":
			function_name += " const char*"
		else:
			function_name += " " + a[1]
		function_name += " " + a[0] + ","	

	filestring = function_name
	filestring = filestring.rstrip(',')
	filestring += ")\n{\n"
	filestring += TAB_SPACE
	filestring += "char sql[1024] = {0};\n"
	filestring += TAB_SPACE
	filestring += "char dbname[50] = \"" + database + "\";\n"
	filestring += TAB_SPACE
	filestring += "sprintf(sql,\"" + sql + "\""
	for a in args:
		filestring += ", " + a[0]
	filestring += ");\n"

	filestring += TAB_SPACE + "result_t res = client->db_query(dbname, sql);\n"
	filestring += TAB_SPACE + namespace + "_" + name + "_res_t " + "r;\n"
	filestring += TAB_SPACE + "bzero(&r, sizeof(r));\n"
	filestring += TAB_SPACE + "r.code = res.code;\n"
	filestring += TAB_SPACE + "memcpy(r.desc, res.desc.c_str(), res.desc.length());\n"
	if operatetype == "select":
		filestring += TAB_SPACE + "r.rows_count = res.getRowCount();\n\n"
		filestring += TAB_SPACE + "if( r.rows_count > 0 ){\n"
		filestring += TAB_SPACE + TAB_SPACE + "r.rows = new " + namespace + "_" + name + "_" + "field_t[r.rows_count];\n"
		filestring += TAB_SPACE + TAB_SPACE + "bool b;\n"
		filestring += TAB_SPACE + TAB_SPACE + "for(int i = 0; i < r.rows_count; ++i){\n"
		i = 0
		for f in select_fields:
			if f[1] != "string":
				filestring += TAB_SPACE + TAB_SPACE + TAB_SPACE + "b = res.getRowDataAsInt(i, " + str(i) + ", r.rows[i]." + f[0] + ");\n"
			else:
				filestring += TAB_SPACE + TAB_SPACE + TAB_SPACE + "b = res.getRowDataAsStr(i, " + str(i) + ", r.rows[i]." + f[0] + ");\n"
			filestring += TAB_SPACE + TAB_SPACE + TAB_SPACE + "assert(b);\n"
			i = i + 1
		filestring += TAB_SPACE + TAB_SPACE + "}\n"
		filestring += TAB_SPACE + "}\n"
	filestring += TAB_SPACE + "return r;\n}\n\n"
#add async function
	filestring += "void " + namespace + "_" + name + "_" + "async(DBProxyClient* client,"
	for arg in args:
		if arg[1] == "string":
			filestring += " const char*"
		else:
			filestring += " " + arg[1]
		filestring +=  " " + arg[0] + ","
	filestring += " query_call_cb_t func, void* arg)\n"
	filestring += "{\n"
	filestring += TAB_SPACE + "char sql[1024] = {0};\n"
	filestring += TAB_SPACE + "char dbname[50] = \"" + database + "\";\n"
	filestring += TAB_SPACE + "sprintf(sql, \"" + sql + "\""
	for arg in args:
		filestring += ", " + arg[0]
	filestring += ");\n"
	filestring += TAB_SPACE + "client->db_async_query(dbname, sql, func, arg);\n"
	filestring += "}\n\n"
	
	return filestring

def deal_item_php(namespace, item):
	if type(item) != dict:
		print "error"	
	name = item["name"]
	database = item["db"]
	operatetype = item["type"]
	sql = item["sql"]
	args = item["arguments"]
	select_fields = item.get("fields")
	filestring = ""
	if operatetype == "select":
		filestring += "class " + namespace + "_" + name + "_field\n{\n"
		for a in select_fields:
			filestring += "public $" + a[0]
			if a[1] == "string":
				filestring += " = \"\";\n"
			else:
				filestring += " = 0;\n"
		filestring += "}\n\n"

	filestring += "class " +  namespace + "_" + name + "_res\n{\n"
	filestring += "public $code = 0;\n"
	filestring += "public $desc = \"\";\n"
	if operatetype == "select":
		filestring += "public $rows_count = 0;\n"
		filestring += "public $row = array();\n"
	filestring += "}\n\n"
#add func name	
	filestring += "function " + namespace + "_" + name + "("
	for a in args:
		filestring += "$" + a[0] + ","
	filestring = filestring.rstrip(',')
	filestring += ")\n"
	filestring += "{\n"
#add func body	
	filestring += TAB_SPACE + "$sql = sprintf(\"" + sql + "\""
	for a in args:
		filestring += ", " + "$" + a[0]
	filestring += ");\n"
	filestring += TAB_SPACE + "$dbname = " + "\"" + database + "\";\n"
	filestring += TAB_SPACE + "$client = new DBProxyClient();\n"
#todo	filestring += TAB_SPACE + "$client->setDbServer(\"" + 
	filestring += TAB_SPACE + "$client_res = $client->doQuery($dbname, $sql);\n"
	filestring += TAB_SPACE + "$res = new " + namespace + "_" + name + "_res();\n"
	filestring += TAB_SPACE + "$res->code = $client_res->code;\n"
	filestring += TAB_SPACE + "$res->desc = $client_res->desc;\n"
	if operatetype == "select":
		filestring += TAB_SPACE + "$res->rows_count = count($client_res->rows);\n"
		filestring += TAB_SPACE + "if( $res->rows_count == 0 ) return $res;\n"
		filestring += TAB_SPACE + "for( $i = 0; $i < $res->rows_count; $i++)\n"
		filestring += TAB_SPACE + "{\n"
		filestring += TAB_SPACE + TAB_SPACE + "$line = new " + namespace + "_" + name + "_field();\n"
		index = 0
		for a in select_fields:
			filestring += TAB_SPACE + TAB_SPACE + "$line->" + a[0] + " = "
			if a[1] == "string":
				filestring += "$client_res->rows[$i][" + str(index)+ "];\n"
			else:
				filestring += "(int)$client_res->rows[$i][" + str(index)+"];\n"
			index = index + 1
		filestring += TAB_SPACE + TAB_SPACE + "$res->rows[] = $line;\n"
		filestring += TAB_SPACE + "}\n"

	filestring += TAB_SPACE + "return $res;\n}\n\n"
	return filestring

def generate_header_file(file):
	name, ext = os.path.splitext(file)
	header_file_name = name + "_" + "db.h"
	f = open(file, "r")
	data = f.read()
	d = eval(data)
	if type(d) != dict:
		sys.exit(0)
	namespace = d["namespace"]
	items = d["statements"]
	filestring = add_warning_info()
	filestring += "#ifndef %s_SQL_H\n" % namespace.upper()
	filestring += "#define %s_SQL_H\n" % namespace.upper()
	filestring += "#include \"DBProxyClient.h\"\n\n"
	filestring += "extern \"C\"{\n\n"
	for i in items:
		filestring += deal_items_header(namespace, i)
	filestring += "\n}\n"
	filestring += "#endif"	
	headerfile = open(header_file_name, "w")
	headerfile.write(filestring)
	headerfile.close()
	f.close()
#	print filestring

def generate_source_file(file):
	name, ext = os.path.splitext(file)
	header_file_name = name + "_" + "db.h"
	filestring = add_warning_info()
	filestring += "#include \"" + header_file_name + "\"" + "\n"
	filestring += "#include <assert.h>\n"
	filestring += "#include <string.h>\n\n"
	f = open(file, "r")
	data = f.read()
	d = eval(data)
	if type(d) != dict:
		sys.exit(0)
	namespace = d["namespace"]
	items = d["statements"]
	for i in items:
		filestring += deal_item_source(namespace, i)
	cppfilename = name + "_" + "db.cpp"
	cppfile = open(cppfilename, "w")
	cppfile.write(filestring)
	cppfile.close()
	f.close()

def generate_php_file(file):
	name, ext = os.path.splitext(file)
	php_file_name = name + "_" + "db.php"
	filestring = "<?php require_once(\"Client.php\")?>\n\n"
	filestring += "<?php\n"
	f = open(file, "r")
	data = f.read()
	d = eval(data)
	if type(d) != dict:
		sys.exit(0)
	namespace = d["namespace"]
	items = d["statements"]
	for i in items:
		filestring += deal_item_php(namespace, i)
	filestring += "\n?>"
	phpfile = open(php_file_name, "w")
	phpfile.write(filestring)
	phpfile.close()
	f.close()

if __name__ == "__main__":
	files = find_sqlconf_file()
	print files
	for file in files:
		generate_header_file(file)
		generate_source_file(file)
		generate_php_file(file)
