Title
========================================================
Задание 1.4. Написать на R функцию stepCV, аналогичную stepAIC, но использующую кросс-валидацию для проверки значимости признака. Нужно следовать при этом принципу иерархии — нельзя выкидывать признаки более низкого порядка, если есть признаки более высокого. Функция должна работать со всеми методами, с которыми работает stepAIC.

Hint: Можно расковырять оригинальную функцию stepAIC, а кросс-валидацию взять из e1071.


```r
library(MASS)
library(e1071)
```


```r
my_data <- read.table("data/seeds_dataset.txt")#, sep = ",", comment.char = "#")
print(colnames(my_data))
```

```
## [1] "V1" "V2" "V3" "V4" "V5" "V6" "V7" "V8"
```


```r
quine.hi <- aov(log(Days + 2.5) ~ .^4, quine)
quine.nxt <- update(quine.hi, . ~ . - Eth:Sex:Age:Lrn)
quine.stp <- stepAIC(quine.nxt,
    scope = list(upper = ~Eth*Sex*Age*Lrn, lower = ~1),
    trace = FALSE)
quine.stp$anova
```

```
## Stepwise Model Path 
## Analysis of Deviance Table
## 
## Initial Model:
## log(Days + 2.5) ~ Eth + Sex + Age + Lrn + Eth:Sex + Eth:Age + 
##     Eth:Lrn + Sex:Age + Sex:Lrn + Age:Lrn + Eth:Sex:Age + Eth:Sex:Lrn + 
##     Eth:Age:Lrn + Sex:Age:Lrn
## 
## Final Model:
## log(Days + 2.5) ~ Eth + Sex + Age + Lrn + Eth:Sex + Eth:Age + 
##     Eth:Lrn + Sex:Age + Sex:Lrn + Age:Lrn + Eth:Sex:Lrn + Eth:Age:Lrn
## 
## 
##            Step Df Deviance Resid. Df Resid. Dev       AIC
## 1                                 120   64.09900 -68.18396
## 2 - Eth:Sex:Age  3 0.973869       123   65.07287 -71.98244
## 3 - Sex:Age:Lrn  2 1.526754       125   66.59962 -72.59652
```

```r
cpus1 <- cpus
for(v in names(cpus)[2:7])
  cpus1[[v]] <- cut(cpus[[v]], unique(quantile(cpus[[v]])),
                    include.lowest = TRUE)
cpus0 <- cpus1[, 2:8]  # excludes names, authors' predictions
cpus.samp <- sample(1:209, 100)
cpus.lm <- lm(log10(perf) ~ ., data = cpus1[cpus.samp,2:8])
cpus.lm2 <- stepAIC(cpus.lm, trace = FALSE)
cpus.lm2$anova
```

```
## Stepwise Model Path 
## Analysis of Deviance Table
## 
## Initial Model:
## log10(perf) ~ syct + mmin + mmax + cach + chmin + chmax
## 
## Final Model:
## log10(perf) ~ mmax + cach + chmin + chmax
## 
## 
##     Step Df   Deviance Resid. Df Resid. Dev       AIC
## 1                             82   2.989974 -314.9906
## 2 - mmin  3 0.09329905        85   3.083273 -317.9179
## 3 - syct  3 0.11044221        88   3.193715 -320.3985
```

```r
example(birthwt)
```

```
## 
## brthwt> bwt <- with(birthwt, {
## brthwt+ race <- factor(race, labels = c("white", "black", "other"))
## brthwt+ ptd <- factor(ptl > 0)
## brthwt+ ftv <- factor(ftv)
## brthwt+ levels(ftv)[-(1:2)] <- "2+"
## brthwt+ data.frame(low = factor(low), age, lwt, race, smoke = (smoke > 0),
## brthwt+            ptd, ht = (ht > 0), ui = (ui > 0), ftv)
## brthwt+ })
## 
## brthwt> options(contrasts = c("contr.treatment", "contr.poly"))
## 
## brthwt> glm(low ~ ., binomial, bwt)
## 
## Call:  glm(formula = low ~ ., family = binomial, data = bwt)
## 
## Coefficients:
## (Intercept)          age          lwt    raceblack    raceother  
##     0.82302     -0.03723     -0.01565      1.19241      0.74068  
##   smokeTRUE      ptdTRUE       htTRUE       uiTRUE         ftv1  
##     0.75553      1.34376      1.91317      0.68020     -0.43638  
##       ftv2+  
##     0.17901  
## 
## Degrees of Freedom: 188 Total (i.e. Null);  178 Residual
## Null Deviance:	    234.7 
## Residual Deviance: 195.5 	AIC: 217.5
```

```r
birthwt.glm <- glm(low ~ ., family = binomial, data = bwt)
birthwt.step <- stepAIC(birthwt.glm, trace = FALSE)
birthwt.step$anova
```

```
## Stepwise Model Path 
## Analysis of Deviance Table
## 
## Initial Model:
## low ~ age + lwt + race + smoke + ptd + ht + ui + ftv
## 
## Final Model:
## low ~ lwt + race + smoke + ptd + ht + ui
## 
## 
##    Step Df Deviance Resid. Df Resid. Dev      AIC
## 1                         178   195.4755 217.4755
## 2 - ftv  2 1.358185       180   196.8337 214.8337
## 3 - age  1 1.017866       181   197.8516 213.8516
```

```r
birthwt.step2 <- stepAIC(birthwt.glm, ~ .^2 + I(scale(age)^2)
    + I(scale(lwt)^2), trace = FALSE)
birthwt.step2$anova
```

```
## Stepwise Model Path 
## Analysis of Deviance Table
## 
## Initial Model:
## low ~ age + lwt + race + smoke + ptd + ht + ui + ftv
## 
## Final Model:
## low ~ age + lwt + smoke + ptd + ht + ui + ftv + age:ftv + smoke:ui
## 
## 
##         Step Df  Deviance Resid. Df Resid. Dev      AIC
## 1                               178   195.4755 217.4755
## 2  + age:ftv  2 12.474896       176   183.0006 209.0006
## 3 + smoke:ui  1  3.056805       175   179.9438 207.9438
## 4     - race  2  3.129586       177   183.0734 207.0734
```

```r
quine.nb <- glm.nb(Days ~ .^4, data = quine)
quine.nb2 <- stepAIC(quine.nb)
```

```
## Start:  AIC=1095.32
## Days ~ (Eth + Sex + Age + Lrn)^4
## 
##                   Df    AIC
## - Eth:Sex:Age:Lrn  2 1092.7
## <none>               1095.3
## 
## Step:  AIC=1092.73
## Days ~ Eth + Sex + Age + Lrn + Eth:Sex + Eth:Age + Eth:Lrn + 
##     Sex:Age + Sex:Lrn + Age:Lrn + Eth:Sex:Age + Eth:Sex:Lrn + 
##     Eth:Age:Lrn + Sex:Age:Lrn
## 
##               Df    AIC
## - Eth:Sex:Age  3 1089.4
## <none>           1092.7
## - Eth:Sex:Lrn  1 1093.3
## - Eth:Age:Lrn  2 1094.7
## - Sex:Age:Lrn  2 1095.0
## 
## Step:  AIC=1089.41
## Days ~ Eth + Sex + Age + Lrn + Eth:Sex + Eth:Age + Eth:Lrn + 
##     Sex:Age + Sex:Lrn + Age:Lrn + Eth:Sex:Lrn + Eth:Age:Lrn + 
##     Sex:Age:Lrn
## 
##               Df    AIC
## <none>           1089.4
## - Sex:Age:Lrn  2 1091.1
## - Eth:Age:Lrn  2 1091.2
## - Eth:Sex:Lrn  1 1092.5
```

```r
quine.nb2$anova
```

```
## Stepwise Model Path 
## Analysis of Deviance Table
## 
## Initial Model:
## Days ~ (Eth + Sex + Age + Lrn)^4
## 
## Final Model:
## Days ~ Eth + Sex + Age + Lrn + Eth:Sex + Eth:Age + Eth:Lrn + 
##     Sex:Age + Sex:Lrn + Age:Lrn + Eth:Sex:Lrn + Eth:Age:Lrn + 
##     Sex:Age:Lrn
## 
## 
##                Step Df   Deviance Resid. Df Resid. Dev      AIC
## 1                                       118   167.4535 1095.324
## 2 - Eth:Sex:Age:Lrn  2 0.09746244       120   167.5509 1092.728
## 3     - Eth:Sex:Age  3 0.11060087       123   167.4403 1089.409
```


```r
df <- mtcars
f <- mpg ~ .
l <- lm(f, df)
(l.aic <- stepAIC(l, trace = TRUE))
```

```
## Start:  AIC=70.9
## mpg ~ cyl + disp + hp + drat + wt + qsec + vs + am + gear + carb
## 
##        Df Sum of Sq    RSS    AIC
## - cyl   1    0.0799 147.57 68.915
## - vs    1    0.1601 147.66 68.932
## - carb  1    0.4067 147.90 68.986
## - gear  1    1.3531 148.85 69.190
## - drat  1    1.6270 149.12 69.249
## - disp  1    3.9167 151.41 69.736
## - hp    1    6.8399 154.33 70.348
## - qsec  1    8.8641 156.36 70.765
## <none>              147.49 70.898
## - am    1   10.5467 158.04 71.108
## - wt    1   27.0144 174.51 74.280
## 
## Step:  AIC=68.92
## mpg ~ disp + hp + drat + wt + qsec + vs + am + gear + carb
## 
##        Df Sum of Sq    RSS    AIC
## - vs    1    0.2685 147.84 66.973
## - carb  1    0.5201 148.09 67.028
## - gear  1    1.8211 149.40 67.308
## - drat  1    1.9826 149.56 67.342
## - disp  1    3.9009 151.47 67.750
## - hp    1    7.3632 154.94 68.473
## <none>              147.57 68.915
## - qsec  1   10.0933 157.67 69.032
## - am    1   11.8359 159.41 69.384
## - wt    1   27.0280 174.60 72.297
## 
## Step:  AIC=66.97
## mpg ~ disp + hp + drat + wt + qsec + am + gear + carb
## 
##        Df Sum of Sq    RSS    AIC
## - carb  1    0.6855 148.53 65.121
## - gear  1    2.1437 149.99 65.434
## - drat  1    2.2139 150.06 65.449
## - disp  1    3.6467 151.49 65.753
## - hp    1    7.1060 154.95 66.475
## <none>              147.84 66.973
## - am    1   11.5694 159.41 67.384
## - qsec  1   15.6830 163.53 68.200
## - wt    1   27.3799 175.22 70.410
## 
## Step:  AIC=65.12
## mpg ~ disp + hp + drat + wt + qsec + am + gear
## 
##        Df Sum of Sq    RSS    AIC
## - gear  1     1.565 150.09 63.457
## - drat  1     1.932 150.46 63.535
## <none>              148.53 65.121
## - disp  1    10.110 158.64 65.229
## - am    1    12.323 160.85 65.672
## - hp    1    14.826 163.35 66.166
## - qsec  1    26.408 174.94 68.358
## - wt    1    69.127 217.66 75.350
## 
## Step:  AIC=63.46
## mpg ~ disp + hp + drat + wt + qsec + am
## 
##        Df Sum of Sq    RSS    AIC
## - drat  1     3.345 153.44 62.162
## - disp  1     8.545 158.64 63.229
## <none>              150.09 63.457
## - hp    1    13.285 163.38 64.171
## - am    1    20.036 170.13 65.466
## - qsec  1    25.574 175.67 66.491
## - wt    1    67.572 217.66 73.351
## 
## Step:  AIC=62.16
## mpg ~ disp + hp + wt + qsec + am
## 
##        Df Sum of Sq    RSS    AIC
## - disp  1     6.629 160.07 61.515
## <none>              153.44 62.162
## - hp    1    12.572 166.01 62.682
## - qsec  1    26.470 179.91 65.255
## - am    1    32.198 185.63 66.258
## - wt    1    69.043 222.48 72.051
## 
## Step:  AIC=61.52
## mpg ~ hp + wt + qsec + am
## 
##        Df Sum of Sq    RSS    AIC
## - hp    1     9.219 169.29 61.307
## <none>              160.07 61.515
## - qsec  1    20.225 180.29 63.323
## - am    1    25.993 186.06 64.331
## - wt    1    78.494 238.56 72.284
## 
## Step:  AIC=61.31
## mpg ~ wt + qsec + am
## 
##        Df Sum of Sq    RSS    AIC
## <none>              169.29 61.307
## - am    1    26.178 195.46 63.908
## - qsec  1   109.034 278.32 75.217
## - wt    1   183.347 352.63 82.790
```

```
## 
## Call:
## lm(formula = mpg ~ wt + qsec + am, data = df)
## 
## Coefficients:
## (Intercept)           wt         qsec           am  
##       9.618       -3.917        1.226        2.936
```

```r
(l.cv <- stepCV(df, l, trace = TRUE))
```

```
## Error in eval(expr, envir, enclos): could not find function "stepCV"
```
