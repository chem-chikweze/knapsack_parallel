        // // TODO: faster than implementation 1 but still slow. 915344
        // std::vector<std::function<void()>> batch_tasks;
        // for (int j = 1; j <= capacity; j++)
        // {
        //     batch_tasks.push_back([i, j, &dp, &weights, &values]()
        //                           {
        //                                if (weights[i - 1] <= j)
        //                                {
        //                                    dp[i][j] = std::max(dp[i - 1][j], values[i - 1] + dp[i - 1][j - weights[i - 1]]);
        //                                }
        //                                else
        //                                {
        //                                    dp[i][j] = dp[i - 1][j];
        //                                } });
        // }
        // pool.enqueueBatch(batch_tasks);