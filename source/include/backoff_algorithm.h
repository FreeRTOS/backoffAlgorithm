/*
 * AWS IoT Device SDK for Embedded C V202011.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * @file backoff_algorithm.h
 * @brief Declaration of retry utility functions and constants for exponential backoff with
 * jitter strategy of retry attempts.
 * This library represents the "Full Jitter" backoff strategy explained in the
 * following document.
 * https://aws.amazon.com/blogs/architecture/exponential-backoff-and-jitter/
 *
 */

#ifndef BACKOFF_ALGORITHM_H_
#define BACKOFF_ALGORITHM_H_

/* Standard include. */
#include <stdint.h>

/**
 * @page backoffalgorithm_page Backoff Algorithm
 * @brief Library for calculating backoff of retry attempts using exponential back off and
 * jitter algorithm.
 *
 * @section backoffalgorithm_overview Overview
 * The backoff algorithm library is a set of APIs that aid in retrying with exponential
 * backoff and jitter. Exponential backoff with jitter is strongly recommended
 * for retrying failed actions over the network with servers. Please see
 * https://aws.amazon.com/blogs/architecture/exponential-backoff-and-jitter/ for
 * more information about the benefits with AWS.
 *
 * Exponential backoff with jitter is typically used when retrying a failed
 * connection to the server. In an environment with poor connectivity, a client
 * can get disconnected at any time. A backoff strategy helps the client to
 * conserve battery by not repeatedly attempting reconnections when they are
 * unlikely to succeed.
 *
 * Before retrying the failed communication to the server, there is a delay period.
 * In this delay period, the task that is retrying must sleep for some random amount
 * of milliseconds between 0 and the lesser of the backoff window (related to the retry attempt)
 * and a predefined maximum delay value. The backoff window is doubled with each retry
 * attempt until the maximum delay value is reached.<br>
 *
 * > sleep_ms = random_between( 0, min( 2<sup>attempts_count</sup> * base_ms, maximum_ms ) )
 */

/**
 * @brief Constant to represent unlimited number of retry attempts.
 */
#define BACKOFF_ALGORITHM_RETRY_FOREVER    0

/**
 * @brief Interface for a random number generator.
 * The user should supply the platform-specific random number generator to the
 * library through the @ref BackoffAlgorithm_InitializeParams API function.
 *
 * @note It is recommended that a true random number generator is supplied
 * to the library. The random number generator should be seeded with an entropy
 * source in the system.
 *
 * @return The random number if successful; otherwise a negative value to indicate
 * failure.
 */
typedef int32_t ( * BackoffAlgorithm_RNG_t )();

/**
 * @ingroup backoff_algorithm_enum_types
 * @brief Status for @ref BackoffAlgorithm_GetNextBackoff.
 */
typedef enum BackoffAlgorithmStatus
{
    BackoffAlgorithmSuccess = 0,     /**< @brief The function successfully calculated the next back-off value. */
    BackoffAlgorithmRngFailure = 1,  /**< @brief The function encountered failure in generating random number. */
    BackoffAlgorithmRetriesExhausted /**< @brief The function exhausted all retry attempts. */
} BackoffAlgorithmStatus_t;

/**
 * @ingroup backoff_algorithm_struct_types
 * @brief Represents parameters required for calculating the back-off delay for the
 * next retry attempt.
 */
typedef struct BackoffAlgorithmContext
{
    /**
     * @brief The maximum backoff delay (in milliseconds) between consecutive retry attempts.
     */
    uint16_t maxBackOffDelay;

    /**
     * @brief The total number of retry attempts completed.
     * This value is incremented on every call to #BackoffAlgorithm_GetNextBackoff API.
     */
    uint32_t attemptsDone;

    /**
     * @brief The maximum backoff value (in milliseconds) for the next retry attempt.
     */
    uint16_t nextJitterMax;

    /**
     * @brief The maximum number of retry attempts.
     */
    uint32_t maxRetryAttempts;

    /**
     * @brief The random number generator function used for calculating the
     * backoff value for the next retry attempt.
     */
    BackoffAlgorithm_RNG_t pRng;
} BackoffAlgorithmContext_t;

/**
 * @brief Initializes the context for using backoff algorithm. The parameters
 * are required for calculating the next retry backoff delay.
 * This function must be called by the application before the first new retry attempt.
 *
 * @param[out] pRetryParams The context to initialize with parameters required
 * for the next backoff delay calculation function.
 * @param[in] maxBackOff The maximum backoff delay (in milliseconds) between
 * consecutive retry attempts.
 * @param[in] backOffBase The base value (in milliseconds) of backoff delay to
 * use in the exponential backoff and jitter model.
 * @param[in] maxAttempts The maximum number of retry attempts. Set the value to
 * #BACKOFF_ALGORITHM_RETRY_FOREVER to retry for ever.
 * @param[in] pRng The platform-specific function to use for random number generation.
 * The random number generator should be seeded before calling this function.
 */
/* @[define_backoffalgorithm_initializeparams] */
void BackoffAlgorithm_InitializeParams( BackoffAlgorithmContext_t * pContext,
                                        uint16_t backOffBase,
                                        uint16_t maxBackOff,
                                        uint32_t maxAttempts,
                                        BackoffAlgorithm_RNG_t pRng );
/* @[define_backoffalgorithm_initializeparams] */

/**
 * @brief Simple exponential backoff and jitter function that provides the
 * delay value for the next retry attempt.
 * After a failure of an operation that needs to be retried, the application
 * should use this function to obtain the backoff delay value for the next retry,
 * and then wait for the backoff time period before retrying the operation.
 *
 * @param[in, out] pRetryContext Structure containing parameters for the next backoff
 * value calculation.
 * @param[out] pNextBackOff This will be populated with the backoff value (in milliseconds)
 * for the next retry attempt. The value does not exceed the maximum backoff delay
 * configured in the context.
 *
 * @return #BackoffAlgorithmSuccess after a successful sleep, #BackoffAlgorithmRngFailure for a failure
 * in random number generation, #BackoffAlgorithmRetriesExhausted when all attempts are exhausted.
 */
/* @[define_backoffalgorithm_getnextbackoff] */
BackoffAlgorithmStatus_t BackoffAlgorithm_GetNextBackoff( BackoffAlgorithmContext_t * pRetryContext,
                                                          uint16_t * pNextBackOff );
/* @[define_backoffalgorithm_getnextbackoff] */

#endif /* ifndef BACKOFF_ALGORITHM_H_ */
