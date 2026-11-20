from pymodbus.client import ModbusSerialClient
import time

# === CONFIGURACIÓN MODBUS ===
client = ModbusSerialClient(
    port='/dev/ttyUSB0',
    baudrate=9600,
    parity='N',
    stopbits=1,
    bytesize=8,
    timeout=1
)

if not client.connect():
    print("❌ No se pudo conectar al inversor")
    exit(1)

# === FUNCIÓN DE LECTURA ===
def read_reg(addr, signed=False, scale=1):
    r = client.read_holding_registers(addr, count=1)
    if not r or not hasattr(r, "registers"):
        return None

    val = r.registers[0]

    if signed and val > 32767:
        val -= 65536

    return val * scale

print("📡 Leyendo inversor... (Ctrl+C para salir)\n")

# === LOOP PRINCIPAL ===
while True:
    try:
        soc   = read_reg(0x100)
        battI = read_reg(0x102, signed=True, scale=0.1)

        mppt1 = read_reg(0x10E)
        mppt2 = read_reg(0x10F)
        solar = mppt1 + mppt2

        casaA = abs(read_reg(0x21B))
        casaB = abs(read_reg(0x232))
        casa  = casaA + casaB

        print(
            f"SOC: {soc:>3}% | "
            f"Batería: {battI:>6.1f} A | "
            f"Solar: {solar:>5} W | "
            f"Casa: {casa:>5} W"
        )

        time.sleep(2)

    except KeyboardInterrupt:
        print("\n👋 Saliendo")
        break
