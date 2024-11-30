package com.codefu.refactor.api.aqiestimatorserver.webConfig;

import jakarta.annotation.PostConstruct;
import lombok.Data;
import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.stereotype.Component;

@Component
@Data
@ConfigurationProperties(prefix = "refactor")
public class ApplicationProperties {

  private PulseEcoProperties pulseEco;

  @PostConstruct
  public void init() {
    if (pulseEco.username == null || pulseEco.username.isEmpty() || pulseEco.username.isBlank()) {
      throw new IllegalArgumentException("Pulse eco username property cannot be empty");
    }

    if (pulseEco.password == null || pulseEco.password.isEmpty() || pulseEco.password.isBlank()) {
      throw new IllegalArgumentException("Pulse eco password property cannot be empty");
    }
  }

  @Data
  public static class PulseEcoProperties {
    private String username;
    private String password;
  }
}