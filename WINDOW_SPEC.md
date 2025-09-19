# Especificación de la Ventana del Plugin Cosmo
# Cosmo Plugin Window Specification

## Diseño de la Interfaz / Interface Design

### Dimensiones de Ventana / Window Dimensions
- **Ancho / Width**: 400px
- **Alto / Height**: 300px
- **Redimensionable / Resizable**: Sí / Yes

### Elementos de la Interfaz / Interface Elements

#### 1. Barra de Título / Title Bar
```
┌─────────────────────────────────────┐
│ Plugin Cosmo - [Estado/Status]      │
└─────────────────────────────────────┘
```

#### 2. Panel Principal / Main Panel
```
┌─────────────────────────────────────┐
│ ┌─ Configuración / Configuration ─┐ │
│ │ Frecuencia / Frequency:  [____] │ │
│ │ Ganancia / Gain:        [____] │ │
│ │ Filtro / Filter:        [____] │ │
│ └─────────────────────────────────┘ │
│                                     │
│ ┌─ Estado / Status ───────────────┐ │
│ │ Conectado / Connected: [●]      │ │
│ │ Señal / Signal:       -45 dBm  │ │
│ │ Ruido / Noise:        -78 dBm  │ │
│ └─────────────────────────────────┘ │
│                                     │
│ [Iniciar/Start] [Parar/Stop]        │
└─────────────────────────────────────┘
```

#### 3. Controles Específicos / Specific Controls

##### Grupo de Frecuencia / Frequency Group
- **Campo de entrada numérica** para frecuencia central
- **Botones +/-** para ajuste fino
- **Selector de banda** (HF, VHF, UHF)

##### Grupo de Ganancia / Gain Group  
- **Deslizador** para control de ganancia RF
- **Campo numérico** para entrada directa
- **Checkbox** para AGC automático

##### Grupo de Filtrado / Filter Group
- **Menú desplegable** con tipos de filtro
- **Controles de ancho de banda**
- **Activar/Desactivar** filtros

#### 4. Área de Visualización / Display Area
- **Indicador de nivel de señal** (medidor analógico)
- **Display numérico** de frecuencia actual
- **LED de estado** de conexión

### Funciones por Elemento / Functions by Element

| Elemento / Element | Función / Function |
|-------------------|-------------------|
| Campo Frecuencia / Frequency Field | Establece la frecuencia de trabajo / Sets working frequency |
| Control Ganancia / Gain Control | Ajusta amplificación de señal / Adjusts signal amplification |
| Selector Filtro / Filter Selector | Configura filtrado de señal / Configures signal filtering |
| Botón Iniciar / Start Button | Activa el procesamiento / Activates processing |
| Botón Parar / Stop Button | Detiene el procesamiento / Stops processing |
| Indicador Estado / Status Indicator | Muestra estado de conexión / Shows connection status |

### Mensajes de Estado / Status Messages

#### En Español:
- "Conectado y funcionando correctamente"
- "Desconectado - Verificar hardware"
- "Error de configuración"
- "Iniciando plugin..."
- "Deteniendo operación..."

#### In English:
- "Connected and operating correctly"
- "Disconnected - Check hardware"
- "Configuration error"
- "Starting plugin..."
- "Stopping operation..."

### Tooltips / Ayuda Contextual

Cada control incluye ayuda contextual bilingüe:
- **Hover** sobre elementos muestra descripción
- **F1** abre ayuda completa
- **Menú contextual** con opciones rápidas

*Each control includes bilingual contextual help:*
- **Hover** over elements shows description  
- **F1** opens complete help
- **Context menu** with quick options