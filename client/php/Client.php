<?php require_once("Socket.php")?>

<?php

class Result
{
public	$code = 0;
public	$desc = "";
public	$rows = array();
}

class DBProxyClient
{
private	$ip = "127.0.0.1";
private	$port = 3333;

public	function setDbServer($ip, $port)
	{
		$this->ip = $ip;
		$this->port = $port;	
	}

public	function doQuery($dbname, $sql)	
	{
		$conn = new TcpClient();
		
		$sendstr = $this->formatMessage($dbname, $sql);
		if( !$conn->Connect($this->ip, $this->port) ){

		}
		if( !$conn->Send($sendstr, strlen($sendstr)) ){
		
		}
		$sizestr = "";
		if( !$conn->Recv($sizestr, 4) ){
			echo "conn->Recv error";
		}
		
		$tmp1 = unpack("L", $sizestr);		
		$tmp = pack('I', $tmp1[1]);
		$tmp_arr = unpack('N', $tmp);
		$size = $tmp_arr[1];
		echo "size:$size\n"; 
		if( !$conn->Recv($recvbuf, (int)$size) ){

		}
		
		$r = new Result();	
		if( !$this->parseMessage($recvbuf, $r) ){

		}
		echo $r->code; echo "\n";	
		echo $r->desc; echo "\n";
		foreach( $r->rows as $value ){
			foreach( $value as $v){
				echo "$v"." ";
			}
			echo "\n";
		}
		return $r;
	}

private	function formatMessage($dbname, $sql)
	{
		$str = $dbname.":".$sql;
		$len = strlen($str);
		$tmp_arr = unpack('I', pack('N', $len));
		$tmp = pack("L", $tmp_arr[1]);

		return $tmp.$str;
	}
	
private	function parseMessage($str, &$res)
	{
		$pos_begin = strpos($str, ":");
		if( $pos_begin == false ){
			echo "the string ':' was not found in the string '$str'";
			return false;
		}
		$pos_end = strpos($str, ":", $pos_begin+1);
		if( $pos_end == false ){
			echo "the string ':' was not found in the string '$str'";
			return false;
		}
		$code_str = substr($str, 0, $pos_begin);
		$desc_str = substr($str, $pos_begin + 1, $pos_end - $pos_begin - 1);
		$res->code = (int)$code_str;	
		$res->desc = $desc_str;
		
		$rest_str = substr($str, $pos_end+1, strlen($str) - $pos_end - 1);
		if( empty($rest_str) ) return true;
	// parse line		
		$len = strlen($rest_str);
		if( $rest_str[0] != '{' || $rest_str[$len - 1] != '}' ){
			echo "return msg format error";
			return false;
		}

		$l_pos = 0;
		echo "rest_str:$rest_str\n";
		while( $l_pos < $len )	
		{
			$r_pos = strpos($rest_str, "}", $l_pos);
			if($r_pos == false ) return false;
			$linestr = substr($rest_str, $l_pos + 1, $r_pos - $l_pos - 1);
			echo "linestr:$linestr\n";
			$line_array = split('\|', $linestr);
			if( $line_array == false ){
				$tmp_array = array();	
				$tmp_array[] = $linestr;
				$res->rows[] = $tmp_array;
				echo $tmp_array[0]; echo "\n";
			}else{
				$res->rows[] = $line_array;
			}
			$l_pos = $r_pos + 1;
		}

		return true;
	}
}
/*
//test
$client = new DBProxyClient();
$client->setDbServer("172.16.7.150", 3333);
$client->doQuery("test", "select name, age, address, school, id from student where name = 'yudahai'");
$client->doQuery("test", "insert into student values(\"XieTingfeng\", 40, \"HongKong\", \"HongKongUnivercity\", 2)");
*/
?>
