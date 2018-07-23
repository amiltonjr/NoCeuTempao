<?php
// Atualiza a página a cada 1 minuto
header('Refresh: 60');

// Inclui a biblioteca de funções
include_once('funcoes.php');

// Obtém os dados da estação
$tempCelsius		= tempCelsius();
$tempFahrenheit		= tempFahrenheit();
$umidadeRelativa	= umidadeRelativa();
$indiceCalor		= indiceCalor();
$dados30dias		= getGrafico30Dias();
?>
<!DOCTYPE html>
	<head>
		<title>NO CÉU TEMPÃO</title>
		<link rel="icon" type="image/jpg" href="logo.jpg"/>
		<link rel="stylesheet" type="text/css" href="style.css">
		<meta charset="UTF-8" author="Amilton Fontoura de Camargo Junior, Carlos Henrique dos Santos e Tiago Pagotto" description="Trabalho de estação meteorológica ministrado na disciplina de Oficina de Integração 2">
		
		<script src="Chart.min.js"></script>

		
	</head>
	
	<body>
		<div id="layout">

			<div id="caixaEsq">
				<div id="ContentEsq">
					<p id="momento">TEMPO NO MOMENTO EM:</p>
				</div>	
				<div id="ContentLocal">
					<p>UTFPR - CP</p>
				</div>
				<div id="ContentTem">
					<p class="titulo">Temperatura:</p>
					<h1 class="valor"> <?=$tempCelsius;?>ºC </h1>
				</div>
				<div id="ContentUmi">
					<P class="titulo"> Umidade:</P>
					<h1 class="valor"> <?=$umidadeRelativa;?>% </h1>
				</div>
			</div>

			<div id="caixaMeioT">
				<h6 class="titGraf">Gráfico da temperatura nos últimos 30 dias:</h6>
				<canvas id="grafico" height="200" width="400"></canvas>
			</div>

			<div id="caixaMeioU">
				<h6 class="titGraf">Gráfico da umidade nos últimos 30 dias:</h6>
				<canvas id="grafico2" height="200" width="400"></canvas>
			</div>	
			
			<div id="caixaDirCima">
				<div id="hora">
					<SPAN ID="Clock">00:00</SPAN>
					<SCRIPT LANGUAGE="JavaScript">
					    var Elem = document.getElementById("Clock");
					    function Horario(){ 
					    var Hoje = new Date(); 
					    var Horas = Hoje.getHours(); 
					    if(Horas < 10){ 
					      Horas = "0"+Horas; 
					    } 
					    var Minutos = Hoje.getMinutes(); 
					    if(Minutos < 10){ 
					      Minutos = "0"+Minutos; 
					    } 
					    var Segundos = Hoje.getSeconds(); 
					    if(Segundos < 10){ 
					      Segundos = "0"+Segundos; 
					    } 
					    Elem.innerHTML = Horas+":"+Minutos; 
					    } 
					   window.setInterval("Horario()",1000);
					</SCRIPT>
				</div>

					<div id="data">
						<script type="text/javascript">
							var mydate=new Date()
							var year=mydate.getYear()
							if (year<2000)
							year += (year < 1900) ? 1900 : 0
							var day=mydate.getDay()
							var month=mydate.getMonth()
							var daym=mydate.getDate()
							if (daym<10)
							daym="0"+daym
							var dayarray=new Array("Domingo","Segunda-feira","Terça-feira","Quarta-feira","Quinta-feira","Sexta-feira","Sábado")
							var montharray=new Array(" de Janeiro de "," de Fevereiro de "," de Março de ","de Abril de ","de Maio de ","de Junho de","de 	Julho de ","de Agosto de ","de Setembro de "," de Outubro de "," de Novembro de "," de Dezembro de ")
							document.write("   "+dayarray[day]+", "+daym+" "+montharray[month]+" "+year)
						
		
						</script>
					</div>

				</div>
					
				<div id="caixaDirBaixo">
					<form id="FormBuscaIntervalo" name="BuscarIntervalo" method= "GET" action="historico.html">
						<fieldset>
							<legend>BUSCA DE INTERVALO</legend>
							<div class="LinhaForm">
								<label class="rotulo" for="datai">Data inicial: </label>
								<input class="campo" type ="date" name="dataInic" class="InputFields"></input>
							</div>
							<br>
							<div class="LinhaForm">
								<label class="rotulo" for="dataf">Data final: </label>
								<input class="campo" type ="date" name="dataFim" class="InputFields"></input>
							</div>
							<br>
							<button type="submit" id="BotaoSearch"> BUSCAR </button>
						</fieldset>
					</form>
				</div>						

		</div>	


			<script>
			var _ultimos30diasT = {
					labels : [
					<? for ($i=1; $i<=30; $i++) echo '"'.$i.' Mai",';?>
					<?php
					//foreach ($dados30dias as $d)
						//echo '"'.$d['dia'].' '.$d['mes'].'",';
					?>
					],
					datasets :
					[
						{
							label: "TEMPERATURA",
							fillColor : "rgba(255,99,71,0.01)",
							strokeColor : "rgba(255,99,71,1)",
							pointColor : "rgba(255,99,71,1)",
							pointStrokeColor : "rgba(255,255,255,0)",
							pointHighlightFill : "#fff",
							pointHighlightStroke : "rgba(255,99,71,1)",
							data : [<? for ($i=0; $i<31; $i++) echo mt_rand(15, 31).',';?>
							<?php
							//foreach ($dados30dias as $d)
								//echo $d['temperatura'].',';
							?>
							]
						}
					]
				};

				var _ultimos30diasU = {
					labels : [
					<? for ($i=1; $i<=30; $i++) echo '"'.$i.' Mai",';?>
					<?php
					//foreach ($dados30dias as $d)
						//echo '"'.$d['dia'].' '.$d['mes'].'",';
					?>
					],
					datasets :
					[
						{
							label: "UMIDADE",
							fillColor : "rgba(0,191,255,0.01)",
							strokeColor : "rgba(0,191,255,1)",
							pointColor : "rgba(0,191,255,1)",
							pointStrokeColor : "rgba(255,255,255,0)",
							pointHighlightFill : "#fff",
							pointHighlightStroke : "rgba(0,191,255,1)",
							data : [<? for ($i=0; $i<31; $i++) echo mt_rand(50, 98).',';?>]
						}
					]
				};

			window.onload = function(){
				var ctx = document.getElementById("grafico").getContext("2d");
				window.myLine = new Chart(ctx).Line(_ultimos30diasT, {
				responsive: false,
				maintainAspectRatio: true,
				scaleShowGridLines : true,
				pointDot : true,
				multiTooltipTemplate: "<%=datasetLabel%>: <%=value%>"
				});

			var dtx = document.getElementById("grafico2").getContext("2d");
				window.myLine = new Chart(dtx).Line(_ultimos30diasU, {
				responsive: false,
				maintainAspectRatio: true,
				scaleShowGridLines : true,
				pointDot : true,
				multiTooltipTemplate: "<%=datasetLabel%>: <%=value%>"
				});

			}
			
			</script>
	</body>

	<footer>
		<p id= "info"> © NO CÉU TEMPÃO<br>
				Trabalho ministrado na disciplina de Oficina de Integração 2 
		</p>
	</footer>

</html>