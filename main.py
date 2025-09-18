import sdr_interface
import patterns
import visualization
import cosmosemiotica

def main():
    # Parámetros experimentales (pueden venir de variables de entorno)
    import os
    DURATION = int(os.getenv("DURATION", "10"))
    SAMPLERATE = int(os.getenv("SAMPLERATE", "48000"))
    PATTERN_TYPE = os.getenv("PATTERN_TYPE", "primes")
    BAND_START = int(os.getenv("BAND_START", "15000"))
    BAND_END = int(os.getenv("BAND_END", "17000"))
    BOOST_DB = float(os.getenv("BOOST_DB", "1.0"))
    MAX_PRIME = int(os.getenv("MAX_PRIME", "500"))

    # 1. Generar patrón
    pattern = patterns.generate_pattern(PATTERN_TYPE, SAMPLERATE, DURATION, BAND_START, BAND_END, MAX_PRIME)

    # 2. Inyectar patrón en banda
    injected_signal = sdr_interface.inject_pattern(pattern, DURATION, SAMPLERATE, BAND_START, BAND_END, BOOST_DB)

    # 3. Grabar señal SDR (simulada)
    sdr_interface.save_signal(injected_signal, "grabacion_sdr.wav", SAMPLERATE)

    # 4. Analizar y visualizar
    analysis = cosmosemiotica.analyze_signal("grabacion_sdr.wav", SAMPLERATE)
    visualization.plot_results(analysis, "resultados/")

if __name__ == "__main__":
    main()
