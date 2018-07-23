CREATE TABLE bloco (
  idBloco INT AUTO_INCREMENT,
  nomeBloco VARCHAR(50) NOT NULL,
  PRIMARY KEY (idBloco)
) ENGINE = InnoDB AUTO_INCREMENT = 1;

CREATE TABLE temperatura (
  idTemperatura INT AUTO_INCREMENT ,
  celsius FLOAT NOT NULL,
  fahrenheit FLOAT NOT NULL,
  PRIMARY KEY (idTemperatura)
) ENGINE = InnoDB AUTO_INCREMENT = 1;

CREATE TABLE umidade (
  idUmidade INT AUTO_INCREMENT,
  relativa FLOAT NOT NULL,
  indiceCalor FLOAT NOT NULL,
  PRIMARY KEY (idUmidade)
) ENGINE = InnoDB AUTO_INCREMENT = 1;

CREATE TABLE clima (
  idClima INT AUTO_INCREMENT,
  data DATETIME NULL,
  idBloco INT NOT NULL,
  idTemperatura INT NOT NULL,
  idUmidade INT NOT NULL,
  PRIMARY KEY (idClima),
  INDEX fk_clima_temperatura1_idx (`idTemperatura` ASC),
  INDEX fk_clima_umidade1_idx (`idUmidade` ASC),
  INDEX fk_clima_bloco1_idx (`idBloco` ASC),
  CONSTRAINT fk_clima_temperatura1
    FOREIGN KEY (idTemperatura) REFERENCES temperatura (idTemperatura) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT fk_clima_umidade1
    FOREIGN KEY (idUmidade) REFERENCES umidade (idUmidade) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT fk_clima_bloco1
    FOREIGN KEY (idBloco) REFERENCES bloco (idBloco) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE = InnoDB AUTO_INCREMENT = 1;
