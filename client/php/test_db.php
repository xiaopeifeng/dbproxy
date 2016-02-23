<?php require_once("Client.php")?>

<?php
class samples_insert_test_res
{
public $code = 0;
public $desc = "";
}

function samples_insert_test($f1,$f2,$f3,$f4,$f5)
{
  $sql = sprintf("insert into student values ('%s', %d, '%s', '%s', %d)", $f1, $f2, $f3, $f4, $f5);
  $dbname = "test";
  $client = new DBProxyClient();
  $client_res = $client->doQuery($dbname, $sql);
  $res = new samples_insert_test_res();
  $res->code = $client_res->code;
  $res->desc = $client_res->desc;
  return $res;
}

class samples_update_test_res
{
public $code = 0;
public $desc = "";
}

function samples_update_test($f1)
{
  $sql = sprintf("update student set name = '%s' where id = 1", $f1);
  $dbname = "test";
  $client = new DBProxyClient();
  $client_res = $client->doQuery($dbname, $sql);
  $res = new samples_update_test_res();
  $res->code = $client_res->code;
  $res->desc = $client_res->desc;
  return $res;
}

class samples_select_test_field
{
public $name = "";
public $age = 0;
public $address = "";
public $school = "";
}

class samples_select_test_res
{
public $code = 0;
public $desc = "";
public $rows_count = 0;
public $row = array();
}

function samples_select_test($f1)
{
  $sql = sprintf("select name, age, address, school from student where id = %d", $f1);
  $dbname = "test";
  $client = new DBProxyClient();
  $client_res = $client->doQuery($dbname, $sql);
  $res = new samples_select_test_res();
  $res->code = $client_res->code;
  $res->desc = $client_res->desc;
  $res->rows_count = count($client_res->rows);
  if( $res->rows_count == 0 ) return $res;
  for( $i = 0; $i < $res->rows_count; $i++)
  {
    $line = new samples_select_test_field();
    $line->name = $client_res->rows[$i][0];
    $line->age = (int)$client_res->rows[$i][1];
    $line->address = $client_res->rows[$i][2];
    $line->school = $client_res->rows[$i][3];
    $res->rows[] = $line;
  }
  return $res;
}

$result = samples_select_test(1);
echo $result->code; echo " "; echo $result->desc; echo " "; echo $result->rows_count; echo "\n";
for( $i = 0; $i < $result->rows_count; $i++)
{
	echo "----------------------\n";
	echo $result->rows[$i]->name; echo " ";
	echo $result->rows[$i]->age; echo " ";
	echo $result->rows[$i]->address; echo " ";
	echo $result->rows[$i]->school; echo " \n";
}
$result2 = samples_insert_test("WangShanshan", 10, "Beijing", "QingHuaUnivercity", 2);
echo $result2->code; echo " "; echo $result2->desc; echo "\n";
$result3 = samples_update_test("happynetwork");
echo $result3->code; echo " "; echo $result3->desc; echo "\n";

?>
