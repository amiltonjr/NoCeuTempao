<?php
/********************

BIBLIOTECA DE FUNÇÕES
PROJETO NO CÉU TEMPÃO

********************/

// Inclui o arquivo de configurações
require_once('config.php');

$BD = mysql_pconnect($bdHost, $bdUser, $bdPass) or die ('Erro ao Conectar ao Banco de Dados!');
mysql_select_db($bdName, $BD) or die ('Erro ao Selecionar o Banco de Dados!');
mysql_query("SET NAMES 'utf8'");
mysql_query('SET character_set_connection=utf8');
mysql_query('SET character_set_client=utf8');
mysql_query('SET character_set_results=utf8');

// Função que executa uma query no Banco de Dados
function query($codigo, $mostrarErro=true) {
	if (strlen($codigo) < 5)
		return false;
	
	// Executa as instruções no banco de dados
	$q = mysql_query($codigo);
	
	if (!$q) {
		// Imprime o erro do MySQL
		if ($mostrarErro)
			echo "\n".'Erro MySQL: ' . mysql_error()."\n";
		
		return false;
	}
	
	return $q;
}

// Função que cria as tabelas caso ainda não existam
function criarTabelas() {
	// Faz a leitura do arquivo BD.sql
	$sql = file_get_contents('BD.sql');
	// Separa os comandos
	$sql = explode(';', $sql);
	
	// Executa cada comando individualmente
	for ($i=0; $i<count($sql); $i++)
		query($sql[$i]);
}

// Função que verifica se uma tabela existe no banco de dados
function existeTabela($tabela) {
	// Consulta no banco
	$q = query("SELECT * FROM `$tabela` LIMIT 1", false);
	
	// Se a tabela não existe
	if ($q != false)
		return true;
	else
		return false;
}

// Função que define o IP da estação meteorológica
function setIP($ip='') {
	global $IP;
	
	if (strlen($ip) >= 7 && strlen($ip) <= 15)
		$IP = $ip;
}

// Função que faz a leitura de uma página
function getContents($url='') {
	// Valida a entrada
	if (strlen($url) < 7) return '';
	
	// Inicia o cURL
	$ch = curl_init();
	$timeout = 5; // Define o timeout (em segundos)
	
	// Define as opções
	curl_setopt($ch, CURLOPT_URL, $url);
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
	curl_setopt($ch, CURLOPT_TIMEOUT, $timeout*1000);
	
	// Executa o cURL
	$res = curl_exec($ch);
	curl_close($ch);
	
	// Retorna o resultado
	return $res;
}

// Função que obtém um array com os dados da estação metereológica
function arrayDados($daEstacao=false) {
	// Se é para puxar os dados da estação
	if ($daEstacao) {
		
		global $IP;
		
		// Verifica se o IP é válido
		if ($IP == '0.0.0.0') return null;
		
		// Faz a leitura dos dados XML
		$html = getContents('http://'.$IP.'/');
		// Converte em um objeto XML
		try {
			$xml = new SimpleXMLElement($html);
		} catch (Exception $e) {
			return null;
		}
		
		// Converte o objeto XML parra Array
		$res = json_decode(json_encode($xml), true);
		
		// Retorna com o array de dados
		return $res;
		
	// Se é para puxar os dados do banco
	} else {
		
		// Puxa o último registro do banco de dados
		$r = mysql_fetch_array(query("SELECT * FROM (SELECT b.nomeBloco, c.data, t.celsius, t.fahrenheit, u.relativa, u.indiceCalor FROM bloco b INNER JOIN clima c INNER JOIN temperatura t INNER JOIN umidade u GROUP BY c.data) s ORDER BY s.data DESC LIMIT 1"));
		
		// Converte os dados do banco para o formato esperado pela aplicação
		$res = array();
		$res['temperatura']['celsius']		= $r['celsius'];
		$res['temperatura']['fahrenheit']	= $r['fahrenheit'];
		$res['umidade']['relativa']			= $r['relativa'];
		$res['umidade']['indice_calor']		= $r['indiceCalor'];
		
		// Retorna com o array de dados
		return $res;
		
	}
}

// Função que retorna o valor correto em float
function Valor($val='') {
	return number_format(floatval($val), 1);
}

// Função que retorna a temperatura em graus Célsius
function tempCelsius() {
	// Obtém o array de dados
	$dados = arrayDados();
	
	// Se é inválido, retorna 0
	if (!is_array($dados)) return 0.0;
	
	// Retorna a temperatura
	return Valor($dados['temperatura']['celsius']);
}

// Função que retorna a temperatura em graus Fahrenheit
function tempFahrenheit() {
	// Obtém o array de dados
	$dados = arrayDados();
	
	// Se é inválido, retorna 0
	if (!is_array($dados)) return 0.0;
	
	// Retorna a temperatura
	return Valor($dados['temperatura']['fahrenheit']);
}

// Função que retorna a umidade relativa do ar em porcentagem
function umidadeRelativa() {
	// Obtém o array de dados
	$dados = arrayDados();
	
	// Se é inválido, retorna 0
	if (!is_array($dados)) return 0.0;
	
	// Retorna a umidade
	return Valor($dados['umidade']['relativa']);
}

// Função que retorna o índice de calor
function indiceCalor() {
	// Obtém o array de dados
	$dados = arrayDados();
	
	// Se é inválido, retorna 0
	if (!is_array($dados)) return 0.0;
	
	// Retorna o índice
	return Valor($dados['umidade']['indice_calor']);
}

// Função que reotrna com o nome do mês
function nomeMes($mes='') {
	// Valida a entrada
	if ($mes == '' || $mes < 1 || $mes > 12) return '';
	
	if ($mes == 1) return 'Jan';
	if ($mes == 2) return 'Fev';
	if ($mes == 3) return 'Mar';
	if ($mes == 4) return 'Abr';
	if ($mes == 5) return 'Mai';
	if ($mes == 6) return 'Jun';
	if ($mes == 7) return 'Jul';
	if ($mes == 8) return 'Ago';
	if ($mes == 9) return 'Set';
	if ($mes == 10) return 'Out';
	if ($mes == 11) return 'Nov';
	else return 'Dez';
}

// Funcão que retorna o vetor para os dados do gráfico de temperatura dos últimos 30 dias
function getGrafico30Dias() {
	$res = array();
	$i = 0;
	
	// Acessa o banco de dados
	$q = query('SELECT * FROM (SELECT b.nomeBloco, c.data AS data, DAY(c.data) AS dia, MONTH(c.data) AS mes, AVG(t.celsius) AS celsius, AVG(t.fahrenheit) AS fahrenheit, AVG(u.relativa) AS relativa, AVG(u.indiceCalor) AS indiceCalor FROM bloco b INNER JOIN clima c INNER JOIN temperatura t INNER JOIN umidade u WHERE data BETWEEN NOW() - INTERVAL 30 DAY AND NOW() GROUP BY c.data) AS k ORDER BY data ASC');
	
	// Percorre os resultados
	while ($r = mysql_fetch_array($q)) {
		$res[$i]['dia'] = $r['dia'];
		$res[$i]['mes'] = nomeMes($r['mes']);
		$res[$i]['temperatura'] = number_format((float)$r['celsius'], 1, '.', '');
		$i++;
	}
	
	// Retorna com os dados do gráfico
	return $res;
}

// Função que cadastra uma leitura da estação no banco de dados
function registrarBD() {
	global $BLOCO;
	
	// Obtém os dados da estação
	$tempCelsius		= tempCelsius();
	$tempFahrenheit		= tempFahrenheit();
	$umidadeRelativa	= umidadeRelativa();
	$indiceCalor		= indiceCalor();
	
	// Se tudo for zero, cancela o salvamento (não houve leitura)
	if ($tempCelsius == 0 && $tempFahrenheit == 0 && $umidadeRelativa == 0 && $indiceCalor == 0)
		return;
	
	// Verifica se o bloco está cadastrado. Caso contrário, cadastra-o
	if (mysql_num_rows(query("SELECT idBloco FROM bloco WHERE nomeBloco = '{$BLOCO}' LIMIT 1")) == 0)
		query("INSERT INTO bloco(nomeBloco) VALUE('{$BLOCO}')");
	
	// Salva o registro de temperatura no BD
	query("INSERT INTO temperatura(celsius, fahrenheit) VALUES('{$tempCelsius}', '{$tempFahrenheit}')");
	query("INSERT INTO umidade(relativa, indiceCalor) VALUES('{$umidadeRelativa}', '{$indiceCalor}')");
	query("INSERT INTO clima(data, idBloco, idTemperatura, idUmidade) VALUES(NOW(), (SELECT idBloco FROM bloco WHERE nomeBloco = '{$BLOCO}' LIMIT 1), (SELECT idTemperatura FROM temperatura ORDER BY idTemperatura DESC LIMIT 1), (SELECT idUmidade FROM umidade ORDER BY idUmidade DESC LIMIT 1))");
}

///////////////////////////////

// Verifica se existem as tabelas no Banco de Dados, cria caso contrário
if ( !existeTabela('clima') )
	criarTabelas();

/*

SELECT b.nomeBloco, c.data, t.celsius, t.fahrenheit, u.relativa, u.indiceCalor FROM bloco b INNER JOIN clima c INNER JOIN temperatura t INNER JOIN umidade u GROUP BY c.data

SELECT b.nomeBloco, c.data, t.celsius, t.fahrenheit, u.relativa, u.indiceCalor FROM bloco b INNER JOIN clima c INNER JOIN temperatura t INNER JOIN umidade u WHERE data BETWEEN NOW() - INTERVAL 10 MINUTE AND NOW() GROUP BY c.data

SELECT * FROM (SELECT b.nomeBloco, c.data, t.celsius, t.fahrenheit, u.relativa, u.indiceCalor FROM bloco b INNER JOIN clima c INNER JOIN temperatura t INNER JOIN umidade u GROUP BY c.data) s ORDER BY s.data DESC LIMIT 1

*/

?>