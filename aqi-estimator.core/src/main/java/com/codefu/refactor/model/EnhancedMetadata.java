package com.codefu.refactor.model;

import jakarta.persistence.*;
import lombok.Data;

import java.time.LocalDateTime;



@SequenceGenerator(name = "ENHANCED_METADATA_SEQUENCE", sequenceName = "ENHANCED_METADATA_SEQUENCE", allocationSize = 1)
public class EnhancedMetadata {

  @Id
  @GeneratedValue(strategy = GenerationType.SEQUENCE, generator = "ENHANCED_METADATA_SEQUENCE")
  private Long id;

  @Column(nullable = false)
  private Integer hour;

  @Column(nullable = false)
  private Double fogImpactIndex;

  @Column(nullable = false)
  private Double airLightRed;

  @Column(nullable = false)
  private Double airLightGreen;

  @Column(nullable = false)
  private Double airLightBlue;

  @Column(nullable = false)
  private Double originalRedMean;

  @Column(nullable = false)
  private Double originalRedVariance;

  @Column(nullable = false)
  private Double originalRedSkewness;

  @Column(nullable = false)
  private Double originalGreenMean;

  @Column(nullable = false)
  private Double originalGreenVariance;

  @Column(nullable = false)
  private Double originalGreenSkewness;

  @Column(nullable = false)
  private Double originalBlueMean;

  @Column(nullable = false)
  private Double originalBlueVariance;

  @Column(nullable = false)
  private Double originalBlueSkewness;

  @Column(nullable = false)
  private Double originalRedToGreen;

  @Column(nullable = false)
  private Double originalRedToBlue;

  @Column(nullable = false)
  private Double originalGreenToBlue;

  @Column(nullable = false)
  private Double deHazedRedMean;

  @Column(nullable = false)
  private Double deHazedRedVariance;

  @Column(nullable = false)
  private Double deHazedRedSkewness;

  @Column(nullable = false)
  private Double deHazedGreenMean;

  @Column(nullable = false)
  private Double deHazedGreenVariance;

  @Column(nullable = false)
  private Double deHazedGreenSkewness;

  @Column(nullable = false)
  private Double deHazedBlueMean;

  @Column(nullable = false)
  private Double deHazedBlueVariance;

  @Column(nullable = false)
  private Double deHazedBlueSkewness;

  @Column(nullable = false)
  private LocalDateTime createdAt;
}