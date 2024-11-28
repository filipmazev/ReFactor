package com.codefu.refactor.model;

import jakarta.persistence.*;
import lombok.Data;

import java.time.LocalDateTime;

@SequenceGenerator(name = "AQI_SEQUENCE", sequenceName = "AQI_SEQUENCE", allocationSize = 1)
@Data
public class AQIModel {

  @Id
  @GeneratedValue(strategy = GenerationType.SEQUENCE, generator = "AQI_SEQUENCE")
  private Long id;

  @Column(nullable = false)
  private Integer aqiEstimation;

  @Column(nullable = false)
  private Integer aqiActual;

  @Column(nullable = false)
  private Double estimateAccuracy;

  @Column(nullable = false)
  private LocalDateTime createdAt;

  @OneToOne
  private EnhancedMetadata enhancedMetadata;
}