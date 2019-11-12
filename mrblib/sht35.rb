class SHT35

  def initialize(iic_addr = 0x45)
    @iic_addr = iic_addr
    init(iic_addr)
  end

  def get_temperature
    get_temperature_with_addr(@iic_addr)
  end

  def get_humidity
    get_humidity_with_addr(@iic_addr)
  end

  def get_temp_and_humi
    get_temp_and_humi_with_addr(@iic_addr)
  end

end
