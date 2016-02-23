<?php 

class TcpClient
{
	var $inited;
	var $connected;
	var $socket;
	var $lastError;
	
	function TcpClient()
	{
		$this->inited = true;
		$this->connected = false;
		$this->lastError = "no error";
		$this->socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
		if ($this->socket < 0)
		{
			$this->lastError = "socket_create() failed reason: ".socket_strerror($this->socket).socket_strerror(socket_last_error());
			$this->inited = false;
			$this->socket = -1;
			return;
		}
		if ( !$this->SetBlock(true) ) return;
	}

private function SetBlock( $block )
	{
		if ( !$this->inited ) return $this->inited;
		$result = true;
		if ($block) $result = socket_set_block($this->socket);
		else $result = socket_set_nonblock($this->socket);
		if ( !$result ) $this->lastError = "SetMode() failed reason: ".socket_strerror(socket_last_error());
		
		return $result;
	}
	 
	function Connect( $ip, $port )
    	{
		if ( !$this->inited ) return $this->inited;
		if ( $this->connected ) return $this->connected;

		if ( socket_connect($this->socket, $ip, $port) ) 
		{
			$this->connected = true;
			return true;
		}
		echo "connect (".$ip.",".$port.") failed\n";	
		$this->lastError = "Connect(".$ip.",".$port.") failed reason:".socket_strerror(socket_last_error());
		return false;
    	}

 	function Send( $msg, $size )
  	{
		if ( !$this->inited ) return $this->inited;
		if ( $size > socket_send( $this->socket, $msg, $size, 0 ) )
		{
			$this->lastError = "Send() failed: reason: ".socket_strerror(socket_last_error());
			return false;
		}
		
		return true;
	}

	function Recv( &$msg, $size, $delFromBuf = true )
	{
		if ( !$this->inited ) return $this->inited;
		$flag = 2;
		if ( $delFromBuf ) $flag = 8;
		$recvSize = socket_recv( $this->socket, $msg, $size, $flag );
		echo "received size:$recvSize"."\n";
		if ( $size > $recvSize )
		{
			$this->lastError = "Recv() failed: reason: ".socket_strerror(socket_last_error());
			echo "recv:(".$recvSize.")".$this->lastError."</br>";
			return false;
		}

		return true;
	}
}

?>
