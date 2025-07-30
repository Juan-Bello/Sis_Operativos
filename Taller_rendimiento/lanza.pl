#!/usr/bin/perl

$Path = `pwd`; #imprime la carpeta en la que se encuentra
chomp($Path); #chomp quita el salto de linea. toma solamente el valor

#nombre de los ejecutables
@Nombre_Ejecutable = ("mmClasicaFork", "mmClasicaOpenMP", "mmClasicaPosix");

#definicion del tamaño de las matrices
@Size_Matriz = ("500","1000","2000");

#definicion de numero de hilos que se usaran
@Num_Hilos = (1,2,4,8);

$Repeticiones = 30;

#bucle que itera en cada dato del arreglo de nombre de ejecutables
foreach $Nombre_Ejecutable (@Nombre_Ejecutable) {
    # bucle que itera en cada tamaño
    foreach $size (@Size_Matriz) {
        # bucle que itera cada numero de hilos
        foreach $hilo (@Num_Hilos) {
            # variable $file que contiene la ruta y nombre de un archivo de salida. combina la ruta, el nombre del ejecutable, el tamaño y los hilos.
            $file = "$Path/$Nombre_Ejecutable-" . $size . "-Hilos-" . $hilo . ".dat";

            # ejecuta la cantidad de repeticiones definidas y guarda la informacion que salga del ejecutable
            for ($i = 0; $i < $Repeticiones; $i++) {
                # llama al ejecutable con los diferentes argumentos y guarda la info en el archivo con direccion a $file
                system("$Path/$Nombre_Ejecutable $size $hilo  >> $file");
            }

            # cierra el archivo
            close($file);

            $p = $p + 1;

            # calcular el promedio de los tiempos almacenados en el archivo
	    my $suma = 0;
            my $conteo = 0;
            open(my $fh, '<', $file) or die "No se pudo abrir $file: $!";
	    while (my $linea = <$fh>) {
    		chomp($linea);
    		if ($linea =~ /(\d+)/) {
        		$suma += $1;
        		$conteo++;
    		}
	    }
	    close($fh);

	    # calcular el promedio si hay datos
	    my $promedio = 0;
	    if ($conteo > 0) {
		$promedio = $suma / $conteo;
	    }

	   # aregar el promedio al archivo
	   system("echo 'Promedio: $promedio ms' >> $file");
        }
    }
}

