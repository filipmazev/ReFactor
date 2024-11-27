package com.codefu.refactor.web;

import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.context.request.async.DeferredResult;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

@RestController
public class AQIController {

  private final ExecutorService bakers = Executors.newFixedThreadPool(5);

  @GetMapping("/bake")
  public DeferredResult<String> publisher() {
    DeferredResult<String> output = new DeferredResult<>();
    bakers.execute(() -> {
      try {
        Thread.sleep(3000L);
        output.setResult("Result 1");
        Thread.sleep(2000L);
        output.setResult("Result 2");
      } catch (Exception e) {
        // ...
      }
    });
    return output;
  }
}
